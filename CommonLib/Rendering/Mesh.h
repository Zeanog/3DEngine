#pragma once

#include "System/Typedefs.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "System/List.h"
#include "System/Map.h"
#include "Images/Image.h"
#include "Rendering/Joint.h"
#include "System/Color.h"
#include "Math/Bounds.h"

class AMeshLoader;
class Joint;

namespace Neo {
	class Mesh {
		CLASS_TYPEDEFS(Mesh)

	public:
		class AMaterial {
			CLASS_TYPEDEFS(AMaterial)

		protected:
			Map<ShaderProgram_GLSL*, List<StaticString>>	m_ShaderProgramRequirements;//Channels required per shader program

		public:
			struct Channel {
				Color<Float32>	Color = decltype(Color)::White;
				const Neo::Image* Texture{};

				Channel(const decltype(Color)& c, decltype(Texture) t) {
					Color = c;
					Texture = t;
				}

				virtual void		Bind() const {
					if (Texture) {
						Texture->Bind();
					}
				}
				virtual void		Unbind() const {
					if (Texture) {
						Texture->Unbind();
					}
				}
			};

			Int32	Index = -1;
			UInt32	PolyCount = 0;

			Map<StaticString, Channel*>	ChannelMap;
			List<Channel*>				Channels;

			AMaterial() {
			}

			AMaterial(Int32 i, UInt32 polyCount) : AMaterial() {
				Index = i;
				PolyCount = polyCount;
			}

			Bool	Equals(const AMaterial& rhs) const {
				if (Index != rhs.Index) {
					return false;
				}

				if (PolyCount != rhs.PolyCount) {
					return false;
				}

				if (Channels.Length() != rhs.Channels.Length()) {
					return false;
				}

				for (UInt32 ix = 0; ix < Channels.Length(); ++ix) {
					auto lhsChannel = Channels[ix];
					auto rhsChannel = rhs.Channels[ix];
					if (lhsChannel->Color != rhsChannel->Color) {
						return false;
					}
					if (lhsChannel->Texture != rhsChannel->Texture) {
						return false;
					}
				}

				return true;
			}

			Bool	operator==(const AMaterial& rhs) const {
				return Equals(rhs);
			}

			Bool	operator!=(const AMaterial& rhs) const {
				return !Equals(rhs);
			}

			template<typename... ChannelNames>
			void AddShaderProgram(ShaderProgram_GLSL* program, ChannelNames... names) {
				assert(!m_ShaderProgramRequirements.Contains(program));

				static decltype(m_ShaderProgramRequirements)::TValue requirements;
				std::initializer_list<int>{
					(requirements.Add(names), 0)...
				};
				m_ShaderProgramRequirements.Add(program, requirements);
				requirements.Clear();
			}

			void AddChannel(const StaticString& channelName) {
				ChannelMap.Add(channelName, new AMaterial::Channel(Color<Float32>::White, nullptr));
				Channels.Add(ChannelMap[channelName]);
			}

			void AddChannel(const char* channelName) {
				AddChannel(StaticString(channelName));
			}

			Bool UpdateChannel(const StaticString& channelName, const Neo::Image* texture) {
				try {
					if(!ChannelMap.Contains(channelName)){
						AddChannel(channelName);
					}
					auto channel = ChannelMap[channelName];
					assert(channel && !channel->Texture);//For now only allow one update
					channel->Texture = texture;
					return true;
				}
				catch (...) {
					return false;
				}
			}

			Bool UpdateChannel(const char* channelName, const Neo::Image* texture) {
				return UpdateChannel(StaticString(channelName), texture);
			}

			Bool UpdateChannel(const StaticString& channelName, const Color<Float32>& color) {
				try {
					if (!ChannelMap.Contains(channelName)) {
						AddChannel(channelName);
					}
					auto channel = ChannelMap[channelName];
					assert(channel);
					channel->Color = color;
					return true;
				}
				catch (...) {
					return false;
				}
			}

			Bool UpdateChannel(const char* channelName, const Color<Float32>& color) {
				return UpdateChannel(StaticString(channelName), color);
			}
		};

		class Material : public AMaterial {
			INHERITEDCLASS_TYPEDEFS(Material, AMaterial)

		public:
			Float32			Shininess{};//TODO: Dont think this should be here.  Basically a specular channel

			Material() {
			}

			Material(Int32 i, Int32 polyCount) : TSuper(i, polyCount) {
			}

			Material&	operator=(const Material& rhs) {
				Index = rhs.Index;
				PolyCount = rhs.PolyCount;
				ChannelMap = rhs.ChannelMap;
				Channels = rhs.Channels;
				return *this;
			}
		};

	protected:
		VertexBuffer	m_VertexBuffer;
		IndexBuffer		m_IndexBuffer;

		List<AMaterial*>	m_Materials;

		Skeleton		m_Skeleton;

		List<StaticString> m_AnimNames;
		Map<StaticString, AnimationClip*>	m_AnimationClips;

		Bounds			m_Bounds;

	public:
		Mesh();

		Mesh(const VertexBuffer& vb, const IndexBuffer& ib);

		virtual ~Mesh() {
			for (UInt32 ix = 0; ix < m_Materials.Length(); ++ix) {
				DeletePtr(m_Materials[ix]);
			}
			m_Materials.Clear();
		}

		const Bounds&	GetBounds() const {
			return m_Bounds;
		}

		UInt32 NumClips() const {
			return m_AnimationClips.Size();
		}

		const AnimationClip* GetClip(int index) const {
			auto& name = m_AnimNames[index];
			return m_AnimationClips[name];
		}

		const AnimationClip* GetClip(const StaticString& name) const {
			return m_AnimationClips[name];
		}

		void	PreRender(VertexBuffer::VertexAttributes attribs) const;
		void	PostRender() const;
		Int32	NumMaterials() const;
		const AMaterial*	GetMaterial(int index) const {
			return m_Materials[index];
		}
		Bool	RenderMaterial(int index) const;
		Bool	RenderMaterial(int index, const List<StaticString>& channels) const;
		Bool	Render() const;
		Bool	RenderJoints() const;
		Bool	RenderJoints(const AnimKeyFrame* pKeyFrame) const;

		Bool	UploadData(const AMeshLoader& loader);

		/*Bool	ReadFrom(const Char* path) {
			File file;

			if (!file.Open(path, "rb")) {
				return false;
			}

			UInt32 fileLength = file.Length();
			const Char* encodedJson = STACK_ALLOC(Char, fileLength);
			file.Read(encodedJson, fileLength);

			json_value* root = json_parse(encodedJson, fileLength);
			Bool validRead = ReadFrom(root);
			json_value_free(root);
			root = NULL;
			return validRead;
		}

		Bool	ReadFrom(const String& path) {
			return ReadFrom(path.CStr());
		}

		Bool	ReadFrom(const StaticString& path) {
			return ReadFrom(path.CStr());
		}

		Bool	ReadFrom(json_value* root) {
			json_value* obj = root;

			if (root->type == json_type::json_string) {
				File file;

				file.Open(root->u.string.ptr, "rb");

				UInt32 dataLength = file.Length();
				Char* encodedJson = STACK_ALLOC(Char, dataLength + 1);
				file.Read(encodedJson, dataLength);
				file.Close();

				obj = json_parse(encodedJson, dataLength);
			}
			
			if (obj->type != json_type::json_object) {
				if (obj != root) {
					json_value_free(obj);
				}
				return false;
			}

			IndexBuffer* ib = NULL;
			for (int ix = 0; ix < obj->u.object.length; ++ix) {
				if (!String::StrICmp(obj->u.object.values[ix].name, "vertexBuffer")) {
					if (!m_VertexBuffer.ReadFrom(obj->u.object.values[ix].value)) {
						if (obj != root) {
							json_value_free(obj);
						}
						return false;
					}
				}

				if (!String::StrICmp(obj->u.object.values[ix].name, "indexBuffer")) {
					if (!m_IndexBuffer.ReadFrom(obj->u.object.values[ix].value)) {
						if (obj != root) {
							json_value_free(obj);
						}
						return false;
					}
				}
			}

			if (obj != root) {
				json_value_free(obj);
			}
			return true;
		}*/

		/*Bool	WriteTo(json_value* root) {
			if( !m_VertexBuffer.WriteTo(root) ) {
				return false;
			}

			if( !m_IndexBuffer.WriteTo(root) ) {
				return false;
			}

			return true;
		}*/
	};
};