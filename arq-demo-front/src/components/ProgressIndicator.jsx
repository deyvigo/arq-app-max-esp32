import { useEffect, useState } from "react";
import { Circle } from "./Circle";

export const ProgressIndicator = ({ bpm, active }) => {
  const [progress, setProgress] = useState(0);

  useEffect(() => {
    if (!active) return

    const interval = setInterval(() => {
      if (progress < 60) {
        setProgress(prevProgress => prevProgress + 1);
      } else {
        clearInterval(interval);
      }
    }, 1000); // Cambia este valor para ajustar la velocidad de la animación

    return () => clearInterval(interval);
  }, [active]);

  return (
    <div className="size-80 p-2 relative">
      <div className=" border-4 size-full border-gray-400 rounded-full"></div>
      <Circle progress={progress} />
      <div className="absolute inset-0 flex flex-col items-center justify-center">
        <span className="text-9xl">
          {bpm ? bpm : 0}
        </span>
        <span className=" text-3xl text-gray-400">
          BPM
        </span>
      </div>
    </div>
  );
};

