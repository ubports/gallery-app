/*
 * Copyright (C) 2012 Canonical Ltd
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

function findChild(parent, cond) {
  if (parent.children) {
    for (var ctr = 0; ctr < parent.children.length; ctr++) {
      var child = parent.children[ctr];
      
      if (cond(child))
        return child;
      
      // recurse
      var found = findChild(child, cond);
      if (found)
        return found;
    }
  }
  
  // If the item has a contentItem, search its children, otherwise done.
  return (parent.contentItem) ? findChild(parent.contentItem, cond) : undefined;
}

function gatherChildren(parent, accum, cond) {
  if (parent.children) {
    for (var ctr = 0; ctr < parent.children.length; ctr++) {
      var child = parent.children[ctr];
      
      if (cond(child))
        accum[accum.length] = child;
      
      gatherChildren(child, accum, cond);
    }
  }
  
  if (parent.contentItem)
    gatherChildren(parent.contentItem, accum, cond);
}

function getRectRelativeTo(item, relativeTo) {
  var rect = item.mapToItem(relativeTo, 0, 0);
  rect.width = item.width;
  rect.height = item.height;
  
  return rect;
}

// Useful only for vertically scrolling containers of multiple items
function getVisibleItems(container, relativeTo, cond) {
  var v = [];
  gatherChildren(container.contentItem, v, function visibleCond(child) {
    if (!cond(child))
      return false;
    
    var rect = getRectRelativeTo(child, container);
    
    return ((rect.y + rect.height) >= 0 && rect.y <= container.height);
  });
  
  return v;
}
