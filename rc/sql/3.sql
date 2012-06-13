
-- Photo edit table

CREATE TABLE PhotoEditTable (
  media_id INTEGER PRIMARY KEY REFERENCES MediaTable ON DELETE CASCADE,
  crop_rectangle TEXT
);
