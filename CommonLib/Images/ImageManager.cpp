#include "ImageManager.h"
#include "ImageLoader_RAW.h"
#include "ImageLoader_TGA.h"
#include "ImageLoader_PNG.h"
#include "System/DebugConsole.h"
//#include "ImageLoader_JPG.h"

ImageManager::ImageManager() {
	m_Loaders[StaticString(".raw")] = new ImageLoader_RAW();
	m_Loaders[StaticString(".tga")] = new ImageLoader_TGA();
	m_Loaders[StaticString(".png")] = new ImageLoader_PNG();
	//m_Loaders[StaticString(".jpg")] = new ImageLoader_JPG();
}

ImageManager::~ImageManager() {
	Destroy(m_Loaders);
}

const Neo::Image* ImageManager::Get(const StaticString& path) {
	if (!path.Length()) {
		return NULL;
	}

	Neo::Image* image = m_Images.Find(path);
	if (image) {
		return image;
	}

	Singleton<DebugConsole>::GetInstance()->Write("Loading %s...\n", path.CStr());

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