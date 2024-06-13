CREATE TABLE `scores` (
	`id` text PRIMARY KEY NOT NULL,
	`score` text NOT NULL
);
--> statement-breakpoint
CREATE TABLE `users` (
	`id` integer PRIMARY KEY NOT NULL,
	`username` text NOT NULL,
	`password` text NOT NULL
);
