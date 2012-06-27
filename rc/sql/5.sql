-- Add is_enhanced column to PhotoEditTable to support one-touch auto-enhance

ALTER TABLE PhotoEditTable ADD COLUMN is_enhanced BOOLEAN default false;
