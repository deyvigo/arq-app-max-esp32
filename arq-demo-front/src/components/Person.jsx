import { useAnimations, useGLTF } from '@react-three/drei'
import React, { useEffect, useRef } from 'react'

export const Person = ({ active }) => {
  const group = useRef()
  const { nodes, materials, animations } = useGLTF('../../animation/run_man_inState.glb')
  const { actions } = useAnimations(animations, group)

  console.log("ACTIVE", active)

  useEffect(() => {
    if (actions && active) {
      actions['Armature|mixamo.com|Layer0.001'].play()
    } else {
      actions['Armature|mixamo.com|Layer0.001'].stop()
    }

  }, [actions, active])

  return (
    <group ref={group} dispose={null} position={[0, -2, 0]} scale={[3, 3, 3]} rotation={[0, Math.PI / 2, 0]}>
      <primitive object={nodes.Armature} />
    </group>
  )
}

useGLTF.preload('../../animation/run_man_inState.glb')
