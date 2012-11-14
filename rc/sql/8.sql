-- Optimizations for media table

ALTER TABLE MediaTable ADD COLUMN timestamp INT DEFAULT NULL;
ALTER TABLE MediaTable ADD COLUMN exposure_time INT DEFAULT NULL;
ALTER TABLE MediaTable ADD COLUMN original_orientation INT DEFAULT -1;
