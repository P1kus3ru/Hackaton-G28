import { Elysia, NotFoundError } from 'elysia'
import * as schema from './schema'
import db from './db'
import { loginDto, postScoreDto } from './dataTransferObjects'
import { eq } from 'drizzle-orm'
import { scores } from './plugin'

new Elysia()
  .use(scores)
  .post(
    'login',
    async ({ body, jwt, params, cookie }) => {
      const result = await db.select().from(schema.users).where(eq(schema.users.username, body.username))
      if (!result.length) throw new NotFoundError('user not found')
      const user = result[0]
      console.log(user)
      console.log(result)

      const isMatch = await Bun.password.verify(body.password, user.password)
      if (isMatch) cookie.auth.set({ value: await jwt.sign(params), httpOnly: true, maxAge: 7 * 86400 })
    },
    loginDto
  )
  .listen(3000)
