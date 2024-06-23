export const Card = ({ bpm, avg_bpm, ir, finger }) => {
  return (
    <section className="text-xl">
      <h1>InfraRed: <span>{ ir }</span></h1>
      <h1>Latidos por minuto: <span>{ bpm.toFixed(4) }</span></h1>
      <h1>Promedio: <span>{ avg_bpm }</span></h1>
      <h1>Scaneando: <span>{ finger ? 'true' : 'false' }</span></h1>
    </section>
  )
}