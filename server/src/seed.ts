import db from './db'
import { scores, users } from './schema'

await db.insert(scores).values([
  {
    id: '28971',
    score: '2',
  },
  {
    id: '7868',
    score: '3',
  },
  {
    id: '768987',
    score: '10',
  },
])

await db.insert(users).values({
  password: await Bun.password.hash(process.env.PASSWORD as string),
  username: 'admin',
})
