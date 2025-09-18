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
		struct AMaterial {
			CLASS_TYPEDEFS(AMaterial)

		public:
			struct Channel {
				Color<Float32>	Color = decltype(Color)::White;
				const Neo::Image* Texture = NULL;

				Channel(decltype(Color) c, decltype(Texture) t) {
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
			Int32	PolyCount = 0;

			Map<StaticString, Channel*>	Channels;

			AMaterial() {
				AddChannel("Diffuse");
			}

			AMaterial(Int32 i, Int32 polyCount) : AMaterial() {
				Index = i;
				PolyCount = polyCount;
			}

			void AddChannel(const StaticString& channelName) {
				Channels.Add(channelName, new AMaterial::Channel(Color<Float32>(), NULL));
			}

			void AddChannel(const char* channelName) {
				AddChannel(StaticString(channelName));
			}

			void UpdateChannel(const StaticString& channelName, const Neo::Image* texture) {
				try {
					auto channel = Channels[channelName];
					if (channel) {
						channel->Texture = texture;
					}
				}
				catch (...) {
				}
			}

			void UpdateChannel(const char* channelName, const Neo::Image* texture) {
				UpdateChannel(StaticString(channelName), texture);
			}

			void UpdateChannel(const StaticString& channelName, const Color<Float32>& color) {
				try {
					auto channel = Channels[channelName];
					if (channel) {
						channel->Color = color;
					}
				}
				catch (...) {
				}
			}

			void UpdateChannel(const char* channelName, const Color<Float32>& color) {
				UpdateChannel(StaticString(channelName), color);
			}

			virtual void		Bind(List<StaticString> channelNames) const {
				/*if (DiffuseTexture) {
					DiffuseTexture->Bind();
				}*/
				FOREACH(iter, channelNames) {
					auto channel = Channels[*iter];
					if (channel) {
						channel->Bind();
					}
				}
			}
			virtual void		Bind() const {
				/*if (DiffuseTexture) {
					DiffuseTexture->Bind();
				}*/
				FOREACH_CONST(iter, Channels) {
					if (iter->second) {
						iter->second->Bind();
					}
				}
			}
			virtual void		Unbind() const {
				/*if (DiffuseTexture) {
					DiffuseTexture->Unbind();
				}*/
				FOREACH_CONST(iter, Channels) {
					if (iter->second) {
						iter->second->Unbind();
					}
				}
			}
		};

		struct Material : public AMaterial {
			INHERITEDCLASS_TYPEDEFS(Material, AMaterial)

		public:
			Float32			Shininess;

			Material() {
				/*Emissive = decltype(Emissive)::Black;
				Ambient = decltype(Ambient)::White;
				Specular = decltype(Specular)::Black;*/
				AddChannel("Emissive");
				AddChannel("Ambient");
				AddChannel("Specular");
				Shininess = 0.0f;
			}

			Material(Int32 i, Int32 polyCount) : TSuper(i, polyCount) {
				AddChannel("Emissive");
				AddChannel("Ambient");
				AddChannel("Specular");
				Shininess = 0.0f;
			}

			Material&	operator=(const Material& rhs) {
				Index = rhs.Index;
				PolyCount = rhs.PolyCount;

				Channels = rhs.Channels;
				
				return *this;
			}

			virtual void		Bind() const override;

			virtual void		Unbind() const override;
		};

	protected:
		VertexBuffer	m_VertexBuffer;
		IndexBuffer		m_IndexBuffer;

		List<AMaterial*>	m_Materials;

		Skeleton		m_Skeleton;

		List<AnimationClip*>	m_AnimationClips;

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

		const AnimationClip* GetClip(int index) const {
			return m_AnimationClips[index];
		}

		void	PreRender(VertexBuffer::VertexAttributes attribs) const;
		void	PostRender() const;
		Int32	NumMaterials() const;
		const AMaterial*	GetMaterial(int index) const {
			return m_Materials[index];
		}
		Bool	RenderMaterial(int index) const;
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

template<>
class JsonSerializer<Neo::Mesh::Material> {
public:
	/*static Vector<_NumDimensions>	ReadFrom(json_value* root) {
		Vector<_NumDimensions>	val;
		ReadFrom(file, val);
		return val;
	}*/

	//static Neo::Mesh::Material	ReadFrom(json_value* root) {
	//	Neo::Mesh::Material outVal;
	//	for (UInt32 ix = 0; ix < root->u.object.length; ++ix) {
	//		if (!String::StrICmp(root->u.object.values[ix].name, "Index")) {
	//			outVal.Index = (Int32)root->u.object.values[ix].value->u.integer;
	//			continue;
	//		}

	//		if (!String::StrICmp(root->u.object.values[ix].name, "PolyCount")) {
	//			outVal.PolyCount = (Int32)root->u.object.values[ix].value->u.integer;
	//			continue;
	//		}

	//		/*if (!String::StrICmp(root->u.object.values[ix].name, "TexturePath")) {
	//			outVal.TexturePath = root->u.object.values[ix].value->u.string.ptr;
	//			continue;
	//		}*/
	//	}

	//	return outVal;
	//}
};