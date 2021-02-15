#pragma once

#include "System/Typedefs.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "System/List.h"

#include "System/File.h"
#include "System/Json.h"
#include "Images/Image.h"
#include "Rendering/Joint.h"
#include "System/Color.h"
#include "Math/Bounds.h"

class AMeshLoader;
class Joint;

namespace Neo {
	class Mesh {
		CLASS_TYPEDEFS(Mesh);

	public:
		struct AMaterialSlot {
			CLASS_TYPEDEFS(AMaterialSlot);

		public:

			Int32	Index;
			Int32	PolyCount;
			const Neo::Image*	Texture;

			AMaterialSlot() {
				Index = -1;
				PolyCount = 0;
				Texture = NULL;
			}

			AMaterialSlot(Int32 i, Int32 polyCount) {
				Index = i;
				PolyCount = polyCount;
				Texture = NULL;
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

		struct MaterialSlot_ColorChannel : public AMaterialSlot {
			INHERITEDCLASS_TYPEDEFS(MaterialSlot_ColorChannel, AMaterialSlot);

		public:
			Color<Float32>	Emissive;
			Color<Float32>	Diffuse;
			Color<Float32>	Ambient;
			Color<Float32>	Specular;
			Float32			Shininess;

			MaterialSlot_ColorChannel() {
				Emissive = decltype(Emissive)::Black;
				Diffuse = decltype(Diffuse)::White;
				Ambient = decltype(Ambient)::White;
				Specular = decltype(Specular)::Black;
				Shininess = 0.0f;
			}

			MaterialSlot_ColorChannel(Int32 i, Int32 polyCount) : TSuper(i, polyCount) {
				Emissive = decltype(Emissive)::Black;
				Diffuse = decltype(Diffuse)::White;
				Ambient = decltype(Ambient)::White;
				Specular = decltype(Specular)::Black;
				Shininess = 0.0f;
			}

			MaterialSlot_ColorChannel&	operator=(const MaterialSlot_ColorChannel& rhs) {
				Index = rhs.Index;
				PolyCount = rhs.PolyCount;
				Texture = rhs.Texture;

				Emissive = rhs.Emissive;
				Diffuse = rhs.Diffuse;
				return *this;
			}

			virtual void		Bind() const override;

			virtual void		Unbind() const override;
		};

	protected:
		VertexBuffer	m_VertexBuffer;
		IndexBuffer		m_IndexBuffer;

		List<AMaterialSlot*>	m_MaterialSlots;

		Skeleton		m_Skeleton;

		List<AnimationClip*>	m_AnimationClips;

		Bounds			m_Bounds;

	public:
		Mesh();

		Mesh(const VertexBuffer& vb, const IndexBuffer& ib);

		virtual ~Mesh() {
			for (UInt32 ix = 0; ix < m_MaterialSlots.Length(); ++ix) {
				DeletePtr(m_MaterialSlots[ix]);
			}
			m_MaterialSlots.Clear();
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
		const AMaterialSlot*	GetMaterialSlot(int index) const {
			return m_MaterialSlots[index];
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
class JsonSerializer<Neo::Mesh::MaterialSlot_ColorChannel> {
public:
	/*static Vector<_NumDimensions>	ReadFrom(json_value* root) {
		Vector<_NumDimensions>	val;
		ReadFrom(file, val);
		return val;
	}*/

	static Neo::Mesh::MaterialSlot_ColorChannel	ReadFrom(json_value* root) {
		Neo::Mesh::MaterialSlot_ColorChannel outVal;
		for (UInt32 ix = 0; ix < root->u.object.length; ++ix) {
			if (!String::StrICmp(root->u.object.values[ix].name, "Index")) {
				outVal.Index = (Int32)root->u.object.values[ix].value->u.integer;
				continue;
			}

			if (!String::StrICmp(root->u.object.values[ix].name, "PolyCount")) {
				outVal.PolyCount = (Int32)root->u.object.values[ix].value->u.integer;
				continue;
			}

			/*if (!String::StrICmp(root->u.object.values[ix].name, "TexturePath")) {
				outVal.TexturePath = root->u.object.values[ix].value->u.string.ptr;
				continue;
			}*/
		}

		return outVal;
	}
};