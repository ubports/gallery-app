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

// Translates a rectangle's coordinates from one item's coordinate space to
// another's.
function translateRect(rect, fromItem, toItem) {
  var newRect = fromItem.mapToItem(toItem, rect.x, rect.y, rect.width, rect.height);
  newRect.width = rect.width;
  newRect.height = rect.height;

  return newRect;
}

// Gets the rect of an item relative to another item.
function getRectRelativeTo(item, relativeTo) {
  return translateRect({"x": 0, "y": 0, "width": item.width, "height": item.height}, item, relativeTo);
}

// Useful only for vertically scrolling containers of multiple items
function getVisibleItems(container, relativeTo, cond) {
  var v = [];
  gatherChildren(container.contentItem, v, function visibleCond(child) {
    if (!cond(child))
      return false;
    
    var rect = getRectRelativeTo(child, relativeTo);
    
    return ((rect.y + rect.height) >= 0 && rect.y <= container.height);
  });
  
  return v;
}

function doesPointIntersectRect(x, y, rect) {
  return x >= rect.x && x <= (rect.x + rect.width) && y >= rect.y && y <= (rect.y + rect.height);
}

function isNumber(n) {
  return n === parseFloat(n);
}

function isEven(n) {
  return isNumber(n) && (n % 2 == 0);
}

function isOdd(n) {
  return isNumber(n) && (n % 2 == 1);
}

function interpolate(start, end, parameter) {
  return start + parameter * (end - start);
}
