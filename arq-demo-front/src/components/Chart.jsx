import { useEffect, useState } from "react"
import ChartLine from "./ChartLine"
import { ChartReadingBpm } from "./ChartReadingBpm"
import { useMessage } from "../hooks/useMessage"

export const Chart = () => {
  const [latidos, setLatidos] = useState([])
  const [latidoMinimo, setLatidoMinimo] = useState(0)
  const [latidoMaximo, setLatidoMaximo] = useState(0)

  const { bpm, avg_bpm } = useMessage()

  console.log("BPM", bpm)


  useEffect(() => {
    setLatidos(prevLatidos => (bpm !== undefined ? [...prevLatidos, bpm] : prevLatidos));
  }, [bpm]);

  useEffect(() => {
    if (latidos.length > 0) {
      setLatidoMaximo(Math.max(...latidos))
      setLatidoMinimo(Math.min(...latidos))
    }
  }, [latidos])

  return (
    <div className='w-full relative'>
      <ChartLine latidos={latidos} />
      <ChartReadingBpm text='Promedio' data={avg_bpm} pos='l' />
      <ChartReadingBpm text='Minimo' data={latidoMinimo.toFixed(2)} pos='c' />
      <ChartReadingBpm text='Maximo' data={latidoMaximo.toFixed(2)} pos='r' />
    </div>
  )
}
