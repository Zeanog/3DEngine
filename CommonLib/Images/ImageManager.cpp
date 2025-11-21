#include "ImageManager.h"
#include "ImageLoader_RAW.h"
#include "ImageLoader_TGA.h"
#include "ImageLoader_PNG.h"
#include "System/DebugConsole.h"
//#include "ImageLoader_JPG.h"

ImageManager::ImageManager() {
	m_Loaders.Add(StaticString(".raw"), new ImageLoader_RAW());
	m_Loaders.Add(StaticString(".tga"), new ImageLoader_TGA());
	m_Loaders.Add(StaticString(".png"), new ImageLoader_PNG());
	//m_Loaders[StaticString(".jpg")] = new ImageLoader_JPG();
}

ImageManager::~ImageManager() {
	Destroy(m_Loaders);
}

const Neo::Image* ImageManager::Get(const StaticString& path) {
	if (!path.Length()) {
		return NULL;
	}

	Neo::Image* image{};
	if (m_Images.Find(path, image)) {
		return image;
	}

	image = new Neo::Image;
	if (!Load(path, image)) {
		DeletePtr(image);
		return NULL;
	}

	m_Images.Add(path, image);
	return image;
}

void ImageManager::ReloadAll() {
	FOREACH(iter, m_Images) {
		Load(iter->first, iter->second);
	}
}

Bool ImageManager::Load(const StaticString& path, Neo::Image* image) {
	assert(image);

	Singleton<DebugConsole>::GetInstance()->Write("Loading '%s'...\n", path.CStr());

	ImageLoader* loader{};
	if (!m_Loaders.Find(StaticString(FilePath::GetExtension(path)), loader) || !loader->Load(path)) {
		Singleton<DebugConsole>::GetInstance()->Write("Failed to load '%s'!\n", path.CStr());
		return false;
	}

	if (!image->UploadData(*loader)) {
		Singleton<DebugConsole>::GetInstance()->Write("Failed to upload '%s'!\n", path.CStr());
		return false;
	}

	loader->Clear();
	return true;
}