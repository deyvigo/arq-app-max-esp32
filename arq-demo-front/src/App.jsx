import { useState, useEffect } from 'react';
import { ProgressIndicator } from './components/ProgressIndicator';
import { Chart } from './components/Chart';
import { CanvasPerson } from './components/CanvasPerson';
import { Alerts } from './components/Alerts';
import { MessageProvider } from './hooks/useMessage';

function App() {

  return (
    <>
      <main className={
        `w-full flex flex-col items-center justify-between min-h-dvh`
      }>
        <h1 className="text-4xl py-8 text-center">Heart Rate App</h1>
        <ProgressIndicator />
        <div className='pt-10 h-[300px]'>
          <CanvasPerson />
        </div>
        <Chart />
      </main >
      <Alerts />
    </>
  )
}

export default App
