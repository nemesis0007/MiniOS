import type { Metadata } from "next";
import MiniOSDashboard from "./MiniOSDashboard";

export const metadata: Metadata = {
  title: "MiniOS Lab",
  description: "Boot and study a real educational x86 operating system in your browser.",
};

export default function Home() {
  return <MiniOSDashboard />;
}
