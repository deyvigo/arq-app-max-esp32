export const ChartReadingBpm = ({ text, data, pos }) => {
  return (
    <article className={`absolute top-32
${pos === 'c' ? 'left-1/2 -translate-x-1/2 mx-auto' : pos === 'r' ? 'right-0' : 'left-0'}`}>
      <h3 className='text-xl underline ' >{text}</h3>
      <div className='flex items-end'>
        <span className='text-5xl'>{data ? data : 0}</span>
        <p className='text-gray-400'>BPM</p>
      </div>
    </article>
  )
}
