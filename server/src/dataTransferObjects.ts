import { t } from 'elysia'

export const postScoreDto = {
  body: t.Object({
    player: t.String(),
    score: t.String(),
  }),
}

export const loginDto = {
  body: t.Object({
    username: t.String(),
    password: t.String(),
  }),
}
