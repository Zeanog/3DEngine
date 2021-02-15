#pragma once

#include "System/AssetManager.h"
#include "Image.h"
#include "System/Singleton.h"

class ImageManager {
	SINGLETON_DECLARATIONS( ImageManager );

public:
	typedef AssetManager<Neo::Image>	TContainer;
	typedef std::map<StaticString, ImageLoader*>	THandlerContainer;

protected:
	TContainer			m_Images;

	THandlerContainer	m_Loaders;

public:
	~ImageManager();

	void		Shutdown() {
		m_Images.Shutdown();
	}

	const Neo::Image*	Get( const Char* path ) {
		return Get( StaticString(path) );
	}

	const Neo::Image*	Get( const String& path ) {
		return Get( path.CStr() );
	}

	const Neo::Image*	Get( const StaticString& path );

	void	ReloadAll();

protected:
	Bool	LoadImage( const StaticString& fp, Neo::Image* image ) {
		assert( image );

		THandlerContainer::iterator iter = m_Loaders.find( StaticString(FilePath::GetExtension(fp)) );
		if( iter == m_Loaders.end() ) {
			return false;
		}

		ImageLoader* loader = iter->second;
		if( !loader->Load(fp) ) {
			return false;
		}

		if( !image->UploadData(*loader) ) {
			return false;
		}

		loader->Clear();
		return true;
	}
};