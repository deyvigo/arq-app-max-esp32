import { useEffect, useState } from "react"
import { AlertIcon } from "../icons/AlertIcon"
import { CloseIcon } from "../icons/CloseIcon"
import { useMessage } from "../hooks/useMessage"

export const Alerts = () => {
  const [alertTItle, setAlertTitle] = useState('')
  const [alertMessage, setAlertMessage] = useState('')

  const { emergencyBPM, emergencyFall, emergencyInactive } = useMessage()

  useEffect(() => {
    if (emergencyFall) {
      setAlertTitle('¡Alerta de caída! Se ha detectado una caída.')
      setAlertMessage('Por favor, revise al usuario inmediatamente.')
    } else if (emergencyBPM) {
      setAlertTitle('¡Alerta de frecuencia cardíaca!')
      setAlertMessage('La frecuencia cardíaca del usuario está fuera del rango normal. Verifique su estado de salud.')
    } else if (emergencyInactive) {
      setAlertTitle('¡Alerta de inactividad! El usuario ha estado inactivo por un período prolongado. ')
      setAlertMessage('Verifique su bienestar.')
    }

    if (!emergencyFall && !emergencyBPM && !emergencyInactive) return

    const dialog = document.getElementById('alert-dialog')
    dialog.showModal()

  }, [emergencyFall, emergencyBPM, emergencyInactive])

  const handleCloseDialog = () => {
    const dialog = document.getElementById('alert-dialog')
    dialog.close()
  }

  return (
    <dialog
      id="alert-dialog"
      className="w-[500px] px-6 py-5 rounded-xl bg-[#242424] fixed gap-4 relative"
    >
      <span
        onClick={handleCloseDialog}
        className="absolute top-2 right-2 cursor-pointer hover:scale-110 transition-transform"
      >
        <CloseIcon />
      </span>
      <AlertIcon className="text-[#F7305A] size-12" />
      <div>
        <h1 className="font-bold py-2 mb-2">{alertTItle}</h1>
        <p>{alertMessage}</p>
      </div>
    </dialog>
  )
}
