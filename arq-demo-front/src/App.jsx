import { useState, useEffect } from 'react';
import { ProgressIndicator } from './components/ProgressIndicator';
import { Chart } from './components/Chart';
import { CanvasPerson } from './components/CanvasPerson';
import { Alerts } from './components/Alerts';

const ws = new WebSocket('ws://192.168.18.11:3000')
// const ws = new WebSocket('ws:localhost:3000')

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

  const {
    ir,
    bpm,
    avg_bpm,
    finger,
    gyro_x,
    gyro_y,
    gyro_z,
    acceleration_x,
    acceleration_y,
    acceleration_z,
    temperature,
    emergencyFall,
    emergencyBPM,
    emergencyInactive,
    systemActive
  } = actualMessage

  return (
    <>
      <main className={
        `w-full flex flex-col items-center justify-between min-h-dvh`
      }>
        <h1 className="text-4xl py-8 text-center">Heart Rate App</h1>
        <ProgressIndicator bpm={bpm} active={finger} />
        <div className='pt-10 h-[300px]'>
          <CanvasPerson active={systemActive} />
        </div>
        <Chart bpm={bpm} avg_bpm={avg_bpm} />
      </main >
      <Alerts emergencyFall={emergencyFall} emergencyBPM={emergencyBPM} emergencyInactive={emergencyInactive} />
    </>
  )
}

export default App
