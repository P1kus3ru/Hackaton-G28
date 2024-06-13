import { integer, text } from 'drizzle-orm/sqlite-core'
import { sqliteTable } from 'drizzle-orm/sqlite-core'

export const scores = sqliteTable('scores', {
  id: text('id').primaryKey().notNull(),
  score: text('score').notNull(),
})

export const users = sqliteTable('users', {
  id: integer('id').primaryKey().notNull(),
  username: text('username').notNull(),
  password: text('password').notNull(),
})
