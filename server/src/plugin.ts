import Elysia from 'elysia'
import { postScoreDto } from './dataTransferObjects'
import db from './db'
import * as schema from './schema'
import jwt from '@elysiajs/jwt'
import { eq } from 'drizzle-orm'

export const plugin = new Elysia({ name: 'plugin' })
  .use(jwt({ name: 'jwt', secret: process.env.JWT_SECRET as string }))
  .onBeforeHandle(async ({ set, jwt, cookie: { auth } }) => {
    const succes = !!(await jwt.verify(auth.value))
    if (!succes) return (set.status = 'Unauthorized')
  })

export const scores = new Elysia()
  .use(plugin)
  .post(
    'scores',
    async ({ body: { player: id, score } }) => {
      try {
        const oldScore = await db.select().from(schema.scores).where(eq(schema.scores.id, id)).execute()
        if (oldScore[0].score < score) {
          await db.update(schema.scores).set({ score }).where(eq(schema.scores.id, id)).execute()
        }
      } catch {
        await db.insert(schema.scores).values({ id, score }).execute()
      }
    },
    postScoreDto
  )
  .get('scores', async () => await db.select().from(schema.scores))
