<script lang="ts" setup>
import type { Score } from '~/types'

defineProps<{ scores: Score[] }>()
</script>

<template>
  <ol>
    <li v-for="score in scores">
      <ScoreRow :player="score.id" :score="score.score" />
    </li>
  </ol>
</template>

<style scoped>
ol {
  display: flex;
  flex-direction: column;
  border: 3px solid var(--text-color);
}
</style>
