/*
 * Copyright (C) 2011 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jim Nelson <jim@yorba.org>
 */

#include "album/album-page.h"

AlbumPage::AlbumPage(int page_number, AlbumTemplatePage* template_page)
  : ContainerSource("Album"), page_number_(page_number), template_page_(template_page) {
}

int AlbumPage::page_number() const {
  return page_number_;
}

AlbumTemplatePage* AlbumPage::template_page() const {
  return template_page_;
}

void AlbumPage::DestroySource(bool destroy_backing) {
}
