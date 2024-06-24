export const Circle = ({ progress }) => {

  const calculateDashArray = () => {
    const radius = 47; // Radio del círculo (mitad del tamaño del círculo)
    const circumference = 2 * Math.PI * radius;
    const dashArray = circumference * (progress / 100);
    return `${dashArray} ${circumference}`;
  };

  return (
    <svg className="absolute inset-0 z-50 transition-all" width="100%" height="100%" viewBox="0 0 100 100">
      <circle
        cx="50"
        cy="50"
        r="47"
        fill="transparent"
        stroke="#7dd3fc"
        strokeWidth="3"
        strokeDasharray={calculateDashArray()}
        strokeDashoffset="0"
        strokeLinecap="round"
        transform="rotate(-90 50 50)"
      />
    </svg>

  )
}
