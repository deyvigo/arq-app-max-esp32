import { FingerprintIcon } from "../icons/FingerprintIcon"

export const MeasurementIndicator = ({ active }) => {
  return (
    <div className="flex flex-col items-center">
      <div
        className={`w-10 h-16 border-2 border-[#F7305A] rounded-full flex items-center justify-center
        ${active && 'bg-[#F7305A]'}
        `}
      >
        <FingerprintIcon className="size-10" />
      </div>
    </div>
  )
}
