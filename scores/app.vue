<script setup lang="ts">
import type { Score } from './types'

useHead({
  title: 'Scores',
  bodyAttrs: {
    class: 'body',
  },
})

const { data: scores } = await useFetch<Score[]>('https://hackaton-bun-sercer.onrender.com/scores')
</script>

<template>
  <div>
    <header>
      <h1>Scores</h1>
    </header>
    <ScoreList v-if="scores" :scores="scores" />
  </div>
</template>

<style scoped>
header {
  display: flex;
  justify-content: center;
}
</style>
