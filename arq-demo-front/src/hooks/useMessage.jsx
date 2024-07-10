import { createContext, useContext, useEffect, useState } from "react";

const MessageContext = createContext()

const ws = new WebSocket('ws://192.168.50.85:3000')
// const ws = new WebSocket('ws:localhost:3000')

export const MessageProvider = ({ children }) => {
  const [message, setMessage] = useState('')

  useEffect(() => {
    const handleReceivedMessage = (event) => {
      const data = JSON.parse(event.data)
      setMessage(data)
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
  } = message

  const value = {
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
  }

  return (
    <MessageContext.Provider value={value}>
      {children}
    </MessageContext.Provider>
  )
}

export const useMessage = () => {
  const context = useContext(MessageContext)
  if (!context) {
    throw new Error('useMessage must be used within a MessageProvider')
  }
  return context
}
