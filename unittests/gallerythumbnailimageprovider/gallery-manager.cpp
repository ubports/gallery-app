#include "core/gallery-manager.h"

GalleryManager* GalleryManager::gallery_mgr_ = NULL;

GalleryManager::GalleryManager()
    : collections_initialised(false),
      startup_timer_(false),
      is_fullscreen_(false),
      is_portrait_(false),
      form_factors_(NULL),
      form_factor_(NULL),
      log_image_loading_(false),
      resource_(NULL),
      gallery_standard_image_provider_(NULL),
      gallery_thumbnail_image_provider_(NULL),
      database_(NULL),
      default_template_(NULL),
      media_collection_(NULL),
      album_collection_(NULL),
      event_collection_(NULL),
      preview_manager_(NULL)
{

}

GalleryManager::~GalleryManager()
{
    delete gallery_mgr_;
    gallery_mgr_ = NULL;
}

GalleryManager* GalleryManager::GetInstance()
{
    if (!gallery_mgr_)
        gallery_mgr_ = new GalleryManager();

    return gallery_mgr_;
}
