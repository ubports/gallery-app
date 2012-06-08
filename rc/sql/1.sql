
-- Media table

CREATE TABLE MediaTable (
  id INTEGER PRIMARY KEY,
  filename TEXT NOT NULL
);

-- Album table

CREATE TABLE AlbumTable (
  id INTEGER PRIMARY KEY,
  time_added INTEGER,
  title TEXT NOT NULL,
  subtitle TEXT NOT NULL
);

CREATE INDEX AlbumTableTimeAddedIndex ON AlbumTable(time_added);

-- Media/album relationship table

CREATE TABLE MediaAlbumTable (
  media_id INTEGER REFERENCES MediaTable ON DELETE CASCADE,
  album_id INTEGER REFERENCES AlbumTable ON DELETE CASCADE
);

CREATE INDEX MediaAlbumTableMediaIndex ON MediaAlbumTable(media_id);
CREATE INDEX MediaAlbumTableAlbumIndex ON MediaAlbumTable(album_id);
