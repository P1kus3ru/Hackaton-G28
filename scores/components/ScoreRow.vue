<script lang="ts" setup>
defineProps<{ player: string; score: number, heade:  }>()
</script>

<template>
  <div class="score-row">
    <div class="header">{{}}</div>
    <div class="player">Player id:{{ player }}</div>
    <div class="score">{{ score }}</div>
  </div>
</template>

<style scoped>
.score-row {
  display: flex;
  justify-content: space-between;
  width: 100%;
  padding: 1rem;
}

.score-row:nth-child() {
  border-top-left-radius: 8px;
}
</style>
