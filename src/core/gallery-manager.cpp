/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Nicolas d'Offay <nicolas.doffay@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "gallery-manager.h"
#include "gallery-application.h"
#include "album/album-collection.h"
#include "album/album-default-template.h"
#include "database/database.h"
#include "database/media-table.h"
#include "event/event-collection.h"
#include "media/media-collection.h"
#include "media/preview-manager.h"
#include "qml/gallery-standard-image-provider.h"
#include "qml/gallery-thumbnail-image-provider.h"
#include "util/resource.h"

GalleryManager* GalleryManager::gallery_mgr_ = NULL;

GalleryManager* GalleryManager::GetInstance()
{
    if (!gallery_mgr_)
        gallery_mgr_ = new GalleryManager();

    return gallery_mgr_;
}

GalleryManager::GalleryManager()
    : collections_initialised(false),
      startup_timer_(false),
      is_fullscreen_(false),
      is_portrait_(false),
      form_factors_(GalleryApplication::instance()->form_factors()),
      form_factor_(GalleryApplication::instance()->form_factor()),
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
    pictures_dir_ = QDir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    process_args();

    resource_ = new Resource(GalleryApplication::instance()->applicationDirPath(), INSTALL_PREFIX);
    gallery_standard_image_provider_ = new GalleryStandardImageProvider();
    gallery_thumbnail_image_provider_ = new GalleryThumbnailImageProvider();
}

void GalleryManager::post_init()
{
    if (!collections_initialised)
    {
        qDebug("Opening %s...", qPrintable(pictures_dir_.path()));

        Exiv2::LogMsg::setLevel(Exiv2::LogMsg::mute);

        database_ = new Database(pictures_dir_, GalleryApplication::instance());
        database_->get_media_table()->verify_files();
        default_template_ = new AlbumDefaultTemplate();
        media_collection_ = new MediaCollection(pictures_dir_);
        album_collection_ = new AlbumCollection();
        event_collection_ = new EventCollection();
        preview_manager_ = new PreviewManager();

        collections_initialised = true;

        qDebug("Opened %s", qPrintable(pictures_dir_.path()));
    }
}

GalleryManager::~GalleryManager()
{
    delete resource_;
    resource_ = NULL;

    delete gallery_standard_image_provider_;
    gallery_standard_image_provider_ = NULL;

    delete gallery_thumbnail_image_provider_;
    gallery_thumbnail_image_provider_ = NULL;

    delete database_;
    database_ = NULL;

    delete default_template_;
    default_template_ = NULL;

    delete media_collection_;
    media_collection_ = NULL;

    delete album_collection_;
    album_collection_ = NULL;

    delete event_collection_;
    event_collection_ = NULL;

    delete preview_manager_;
    preview_manager_ = NULL;
}

void GalleryManager::process_args()
{
  QStringList args = GalleryApplication::instance()->arguments();

  for (int i = 1; i < args.count(); ++i) {
    QString arg = args[i];
    QString value = (i + 1 < args.count() ? args[i + 1] : "");

    if (arg == "--help" || arg == "-h") {
      usage();
    } else if (arg == "--landscape") {
      is_portrait_ = false;
    } else if (arg == "--portrait") {
      is_portrait_ = true;
    } else if (arg == "--fullscreen") {
      is_fullscreen_ = true;
    } else if (arg == "--startup-timer") {
      startup_timer_ = true;
    } else if (arg == "--log-image-loading") {
      log_image_loading_ = true;
    } else {
      QString form_factor = arg.mid(2); // minus initial "--"

      if (arg.startsWith("--") && form_factors_->keys().contains(form_factor)) {
        *form_factor_ = form_factor;
      } else if (arg.startsWith("--desktop_file_hint")) {
        // ignore this command line switch, hybris uses it to get application info
      } else if (i == args.count() - 1 && QDir(arg).exists()) {
        pictures_dir_ = QDir(arg);
      } else {
        invalid_arg(arg);
      }
    }
  }
}

void GalleryManager::usage(bool error)
{
  QTextStream out(error ? stderr : stdout);
  out << "Usage: gallery [options] [pictures_dir]" << endl;
  out << "Options:" << endl;
  out << "  --landscape\trun in landscape orientation (default)" << endl;
  out << "  --portrait\trun in portrait orientation" << endl;
  out << "  --fullscreen\trun fullscreen" << endl;
  foreach (const QString& form_factor, form_factors_->keys())
    out << "  --" << form_factor << "\trun in " << form_factor << " form factor" << endl;
  out << "  --startup-timer\n\t\tdebug-print startup time" << endl;
  out << "  --log-image-loading\n\t\tlog image loading" << endl;
  out << "pictures_dir defaults to ~/Pictures, and must exist prior to running gallery" << endl;
  std::exit(error ? 1 : 0);
}

void GalleryManager::invalid_arg(QString arg)
{
  QTextStream(stderr) << "Invalid argument '" << arg << "'" << endl;
  usage(true);
}
