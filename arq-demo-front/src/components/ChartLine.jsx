import { Line } from 'react-chartjs-2'
import {
  Chart as ChartJS,
  LineElement,
  CategoryScale,
  LinearScale,
  PointElement,
  Filler
} from 'chart.js'

ChartJS.register(LineElement, CategoryScale, LinearScale, PointElement, Filler)

const ChartLine = ({ latidos }) => {
  const generateLabels = () => {
    const labels = []
    for (let i = Math.max(0, latidos.length - 10); i < latidos.length; i++) {
      labels.push(i)
    }
    return labels.slice(-10)
  }


  const data = {
    labels: generateLabels(),
    datasets: [
      {
        label: 'Latidos',
        data: latidos.slice(-10),
        borderColor: 'rgba(75,192,192,1)',
        borderWidth: 2,
        fill: {
          target: 'origin',
          above: 'rgba(75,192,192,.1)',
        },
        pointBorderWidth: 0
      }
    ]
  }

  const options = {
    responsive: true,
    tension: 0.3,
    scales: {
      y: {
        display: false,
        beginAtZero: true,
      },
      x: {
        display: false,
        beginAtZero: true
      }
    },
  }

  return (
    <Line data={data} options={options} />
  )
}

export default ChartLine
