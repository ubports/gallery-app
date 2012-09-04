-- Add orientation column to PhotoEditTable to support rotating files where we
-- don't store the orientation as metadata in the file itself.

ALTER TABLE PhotoEditTable ADD COLUMN orientation INT DEFAULT 0;
