import express, { Response } from 'express'
import cors from 'cors'
import { Server as SocketServer } from 'socket.io'
import http from 'node:http'

const app = express()


app.use(cors({
  origin: '*', // Permitir solo este origen
  methods: ['GET', 'POST', 'OPTIONS'], // Métodos permitidos
  allowedHeaders: ['Content-Type'], // Encabezados permitidos
}))

const server = http.createServer(app)

// Configurar CORS para Socket.io
const io = new SocketServer(server, {
  cors: {
    origin: '*', // Permitir solo este origen
    methods: ['GET', 'POST'], // Métodos permitidos
    allowedHeaders: ['Content-Type'], // Encabezados permitidos
  }
})

io.on('connection', (socket) => {
  socket.on('message', (body) => {
    socket.broadcast.emit('message', body)
  })
})

app.get('/helloworld', (_, res: Response) => {
  res.send({
    "saludo": "Hello World"
  })
})

server.listen(3000, () => {
  console.log('Server on http://localhost:3000')
})
