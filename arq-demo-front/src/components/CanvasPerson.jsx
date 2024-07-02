import { Canvas } from "@react-three/fiber"
import { OrbitControls } from '@react-three/drei';
import { Suspense } from "react";
import { Person } from "./Person";

export const CanvasPerson = ({ active }) => {
  return (
    <Canvas>
      <ambientLight intensity={1} />
      <spotLight position={[10, 10, 10]} angle={0.15} penumbra={1} />
      <pointLight position={[-10, -10, -10]} />
      <Suspense fallback={null}>
        <Person active={active} />
      </Suspense>
      <OrbitControls />
    </Canvas>
  )
}

