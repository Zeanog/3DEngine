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

class IMeshLoader;
class Joint;

namespace Neo {
	class Mesh {
		CLASS_TYPEDEFS(Mesh)

	public:
		class AMaterial {
			CLASS_TYPEDEFS(AMaterial)

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

			struct PolyRange {
				Int32	StartIndex = -1;
				UInt32	VertCount = 0;

				PolyRange() {
				}
				PolyRange(Int32 start, UInt32 numPolys) {
					StartIndex = start;
					VertCount = numPolys * 3;
				}
			};

			StaticString	Name;
			List<PolyRange>	Ranges;

			Map<StaticString, Channel*>	ChannelMap;
			List<Channel*>				Channels;

			AMaterial(const Char* name) {
				Name = name;
			}

			AMaterial(const Char* name, Int32 i, UInt32 numPolys) : AMaterial(name) {
				Ranges[0].StartIndex = i;
				Ranges[0].VertCount = numPolys * 3;
			}

			Bool	Equals(const AMaterial& rhs) const {
				if( Ranges.Length() != rhs.Ranges.Length()) {
					return false;
				}

				for (UInt32 ix = 0; ix < Ranges.Length(); ++ix) {
					auto lhsRange = Ranges[ix];
					auto rhsRange = rhs.Ranges[ix];
					if (lhsRange.StartIndex != rhsRange.StartIndex) {
						return false;
					}
					if (lhsRange.VertCount != rhsRange.VertCount) {
						return false;
					}
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
				auto chan = new AMaterial::Channel(Color<Float32>::White, nullptr);
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
			Float32			Shininess{};//TODO: Dont think this should be here.  Basically a specular channel

			Material(const Char* name) : TSuper(name) {
			}

			Material(const Char* name, Int32 i, Int32 polyCount) : TSuper(name, i, polyCount) {
			}

			Material&	operator=(const Material& rhs) {
				Ranges = rhs.Ranges;
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
		Bool	RenderMaterial(int index) const;
		Bool	RenderMaterial(int index, const List<StaticString>& channels) const;
		Bool	Render() const;
		Bool	RenderJoints() const;
		Bool	RenderJoints(const AnimKeyFrame* pKeyFrame) const;

		Bool	UploadData(const IMeshLoader& loader);
	};
};