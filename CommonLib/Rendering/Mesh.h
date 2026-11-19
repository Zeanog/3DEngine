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
#include "Shaders/ShaderProgram_GLSL.h"

class IMeshLoader;

namespace Neo {
	class Mesh {
		CLASS_TYPEDEFS(Mesh)

	public:
		static const UInt32 TRIANGLE_VERTEX_COUNT = 3U;

		class AMaterial {
			ABSTRACT_CLASS_TYPEDEFS(AMaterial) {}

		public:
			struct Channel {
				Color<Float32>		Color{ decltype(Color)::Invalid };
				const StaticString	ColorName;

				const Neo::Image*	Texture{};

				Channel(const StaticString& channelName, const decltype(Color)& c, decltype(Texture) t): ColorName(String::Format("v%sColor", channelName.CStr())) {//Add Shader prefix and postfix
					Color = c;
					Texture = t;
				}

				virtual void		Bind(UInt32 location, const ShaderProgram_GLSL& program) const {
					if (Color != decltype(Color)::Invalid) {
						program.LinkUniform(ColorName.CStr(), &Color[0], Color.NumComponents);
					}

					if (Texture) {
						Texture->Bind();
					}
				}
				virtual void		Unbind(UInt32 location, const ShaderProgram_GLSL& program) const {
					if (Texture) {
						Texture->Unbind();
					}
				}
			};

			struct PolyRange {
				UInt32	StartIndex = std::numeric_limits<UInt32>::max();
				UInt32	IndexCount = 0;//std::numeric_limits<UInt32>::max();

				PolyRange() {
				}
				PolyRange(UInt32 start, UInt32 numPolys) {
					StartIndex = start;
					IndexCount = numPolys * TRIANGLE_VERTEX_COUNT;//TODO: Fix this assumption
				}

				UInt32	FinalIndex() const {
					assert(StartIndex < std::numeric_limits<UInt32>::max() && IndexCount > 0);
					return (StartIndex + IndexCount) - 1;
				}
			};

			StaticString	Name;
			//List<PolyRange>	Ranges;
			PolyRange		Range;

			Map<StaticString, Channel*>	ChannelMap;
			List<Channel*>				Channels;

			AMaterial(const Char* name) {
				Name = name;
			}

			AMaterial(const Char* name, Int32 i, UInt32 numPolys) : AMaterial(name) {
				Range.StartIndex = i;
				Range.IndexCount = numPolys * 3;//TODO: Fix this assumption
			}

			Bool	Equals(const AMaterial& rhs) const {
				if (Range.StartIndex != rhs.Range.StartIndex) {
					return false;
				}
				if (Range.IndexCount != rhs.Range.IndexCount) {
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

			void AddChannel(const StaticString& channelName) {
				auto chan = new AMaterial::Channel(channelName, Color<Float32>::Invalid, nullptr);
				ChannelMap.Add(channelName, chan);
				Channels.Add(chan);
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
					assert(channel && (!channel->Texture || channel->Texture == texture));//For now only allow one update
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
			INHERITED_CLASS_TYPEDEFS(Material, AMaterial)

		public:
			Material(const Char* name) : TSuper(name) {
			}

			Material(const Char* name, Int32 i, Int32 polyCount) : TSuper(name, i, polyCount) {
			}

			Material&	operator=(const Material& rhs) {
				Range = rhs.Range;
				ChannelMap = rhs.ChannelMap;
				Channels = rhs.Channels;
				return *this;
			}
		};

	protected:
		Neo::VertexBuffer	m_VertexBuffer;
		Neo::IndexBuffer	m_IndexBuffer;

		List<AMaterial*>	m_Materials;

		Neo::Skeleton		m_Skeleton;

		List<AnimationClip*>	m_AnimationClips;

		Bounds				m_Bounds;

	public:
		Neo::VertexBuffer& VertexBuffer() {
			return m_VertexBuffer;
		}

		const Neo::VertexBuffer& VertextBuffer() const {
			return m_VertexBuffer;
		}

		Neo::IndexBuffer& IndexBuffer() {
			return m_IndexBuffer;
		}

		const Neo::IndexBuffer& IndexBuffer() const {
			return m_IndexBuffer;
		}

		List<AMaterial*>& Materials() {
			return m_Materials;
		}

		const List<AMaterial*>& Materials() const {
			return m_Materials;
		}

		List<AnimationClip*>& AnimationClips() {
			return m_AnimationClips;
		}

		Neo::Skeleton& Skeleton() {
			return m_Skeleton;
		}

		Mesh();

		Mesh(const Neo::VertexBuffer& vb, const Neo::IndexBuffer& ib);

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
			return m_AnimationClips.Length();
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
		Bool	RenderMaterial(int index, const ShaderProgram_GLSL& program) const;
		Bool	Render() const;
		Bool	RenderJoints() const;
		Bool	RenderJoints(const AnimKeyFrame* pKeyFrame) const;
	};
};