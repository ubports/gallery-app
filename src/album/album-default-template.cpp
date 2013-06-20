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
 * Charles Lindsay <chaz@yorba.org>
 */

#include "album-default-template.h"

/*!
 * \brief AlbumDefaultTemplate::AlbumDefaultTemplate
 */
AlbumDefaultTemplate::AlbumDefaultTemplate()
    : AlbumTemplate("Default Album Template"),
      m_nextDecisionPageType(LANDSCAPE)
{
    addPage(new AlbumTemplatePage("Template A Left",
                                  "qml/AlbumViewer/AlbumInternals/AlbumPageLayoutLeftPortrait.qml",
                                  true, 1, PORTRAIT));
    addPage(new AlbumTemplatePage("Template A Right",
                                  "qml/AlbumViewer/AlbumInternals/AlbumPageLayoutRightPortrait.qml",
                                  false, 1, PORTRAIT));
    addPage(new AlbumTemplatePage("Template B Left",
                                  "qml/AlbumViewer/AlbumInternals/AlbumPageLayoutLeftDoubleLandscape.qml",
                                  true, 2, LANDSCAPE, LANDSCAPE));
    addPage(new AlbumTemplatePage("Template B Right",
                                  "qml/AlbumViewer/AlbumInternals/AlbumPageLayoutRightDoubleLandscape.qml",
                                  false, 2, LANDSCAPE, LANDSCAPE));
}

/*!
 * \brief AlbumDefaultTemplate::resetBestFitData
 */
void AlbumDefaultTemplate::resetBestFitData()
{
    m_nextDecisionPageType = LANDSCAPE;
}

/*!
 * \brief AlbumDefaultTemplate::getBestFitPage
 * \param isLeft
 * \param numPhotos
 * \param photoOrientations
 * \return
 */
AlbumTemplatePage* AlbumDefaultTemplate::getBestFitPage(bool isLeft,
                                                        int numPhotos, PageOrientation photoOrientations[])
{

    QList<AlbumTemplatePage*> candidates;
    foreach(AlbumTemplatePage* page, pages()) {
        if (page->isLeft() == isLeft)
            candidates.append(page);
    }

    // If the first photo is portrait (or there are no photos to place), we use
    // the page with a portrait slot.
    if (numPhotos < 1 || photoOrientations[0] == PORTRAIT) {
        foreach(AlbumTemplatePage* page, candidates) {
            if (page->framesFor(PORTRAIT) > 0)
                return page;
        }
    }

    // If two landscapes (or just one landscape at the end of the list), use the
    // page with multiple landscape slots.
    if (numPhotos == 1 || photoOrientations[1] != PORTRAIT) {
        foreach(AlbumTemplatePage* page, candidates) {
            if (page->framesFor(LANDSCAPE) > 1)
                return page;
        }
    }

    // In this case, we've got a landscape followed by a portrait.  There's no
    // easy way to handle this, so we flip-flop returning the two possible pages.
    AlbumTemplatePage* selected = NULL;
    foreach(AlbumTemplatePage* page, candidates) {
        if (page->framesFor(m_nextDecisionPageType) > 0) {
            selected = page;
            break;
        }
    }

    m_nextDecisionPageType = (m_nextDecisionPageType == PORTRAIT
                                ? LANDSCAPE : PORTRAIT);
    Q_ASSERT(selected != NULL);
    return selected;
}
