using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;

namespace CheckThreads
{
    public class ThreadChecker
    {
        const string fmt = @"hh\:mm\:ss";

        const double moSize = 1024.0 * 1024.0;

        private readonly int _pid;

        public ThreadChecker(string program)
        {
            if (!int.TryParse(program, out _pid))
            {
                var processes = Process.GetProcessesByName(program).Where(x => IsProcessValid(x));
                if (processes.Count() == 1)
                    _pid = processes.First().Id;
                else
                    throw new Exception($"Found {processes.Count()} matching process name '{program}'");
            }
            DisplayStaticProcessInfos(Process.GetProcessById(_pid));
        }

        private bool IsProcessValid(Process process)
        {
            return !process.HasExited;
        }

        private void DisplayStaticProcessInfos(Process process)
        {
            Console.WriteLine($"Process: {process.ProcessName} pid={process.Id}");
            Console.WriteLine($"Start: {process.StartTime:yyyy/MM/dd hh:mm:ss}");
            for (int i = 0; i < process.Modules.Count; i++)
            {
                var module = process.Modules[i];
                Console.WriteLine($"{module.FileName,-80} S:{Helpers.FD(module.ModuleMemorySize / moSize)} B:{module.BaseAddress.ToString("X16")} E:{module.EntryPointAddress.ToString("X16")}");          
            }
        }


        private void DisplayDynamicProcessInfos(Process process)
        {
            Console.WriteLine($"TotalTime: {process.TotalProcessorTime.ToString(fmt)}");
            Console.WriteLine($"WorkingSet: {process.WorkingSet64 / moSize:.0} Mo");
            Console.WriteLine($"PeakWorkingSet: {process.PeakWorkingSet64 / moSize:.0} Mo");
            Console.WriteLine($"Handle count: {process.HandleCount}");
        }

        public void Check(int delay = 2, int duration = 1_000_000)
        {
            var previousThreads = new SortedDictionary<int, ThreadInfo>();
            var target = DateTime.Now.AddSeconds(duration);
            do
            {
                Console.WriteLine("===========");
                var process = Process.GetProcessById(_pid);
                DisplayDynamicProcessInfos(process);
                Console.WriteLine("===");
                var currentThreads = new SortedDictionary<int, ThreadInfo>();
                for (int i = 0; i < process.Threads.Count; i++)
                {
                    var threadInfo = new ThreadInfo(process.Threads[i]);
                    if (previousThreads.TryGetValue(threadInfo.Id, out ThreadInfo previous))
                        threadInfo.DiffPrevious(previous);
                    currentThreads.Add(threadInfo.Id, threadInfo);
                }
                foreach (var threadInfo in currentThreads)
                {
                    Console.WriteLine(threadInfo.Value);
                }
                
                previousThreads = currentThreads;
                Thread.Sleep(delay * 1000);
            }
            while (DateTime.Now < target);
        }
    }
}
