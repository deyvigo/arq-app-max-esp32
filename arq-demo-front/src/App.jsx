import { useState, useEffect } from 'react'
import { ProgressIndicator } from './components/ProgressIndicator'
import { MeasurementIndicator } from './components/MeasurementIndicator'
import { Chart } from './components/Chart'

const ws = new WebSocket('ws://192.168.0.110:3000')

function App() {
  const [actualMessage, setActualMessage] = useState({})

  useEffect(() => {
    const handleReceivedMessage = (event) => {
      const message = JSON.parse(event.data)
      setActualMessage(message)
    }
    ws.addEventListener('message', handleReceivedMessage)

    return () => {
      ws.removeEventListener('message', handleReceivedMessage)
    }
  }, [])

  console.log("message", actualMessage)

  const { ir, bpm, avg_bpm, finger } = actualMessage

  return (
    <>
      <main className='w-full flex flex-col items-center justify-between min-h-dvh'>
        <h1 className="text-4xl py-8 text-center">Heart Rate App</h1>
        <ProgressIndicator bpm={bpm} active={finger} />
        <MeasurementIndicator active={finger} />
        <Chart bpm={bpm} avg_bpm={avg_bpm} />
      </main >
    </>
  )
}

export default App
