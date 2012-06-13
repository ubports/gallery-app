
-- Album table
-- Add columns for open state and current page

ALTER TABLE AlbumTable ADD COLUMN is_closed BOOLEAN DEFAULT 1;
ALTER TABLE AlbumTable ADD COLUMN current_page INTEGER DEFAULT -1;
