using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CheckThreads
{
    public class ThreadInfo
    {
        const string fmt      = @"hh\:mm\:ss";
        const string fmtDelta = @"ss\:ffffff";

        public int Id { get; set; }

        public TimeSpan TotalProcessorTime { get; set; }

        public DateTime TimeStamp { get; set; }

        // For diff time
        public bool IsDiffSet { get; set; }       
        public double DiffTotalProcessorTime { get; set; }
        public double Load { get; set; }

        public ThreadInfo(ProcessThread processThread)
        {
            Id = processThread.Id;
            TotalProcessorTime = processThread.TotalProcessorTime;
            TimeStamp = DateTime.Now;
        }

        public void DiffPrevious(ThreadInfo previous)
        {
            var deltaTime = TimeStamp - previous.TimeStamp;
            DiffTotalProcessorTime = TotalProcessorTime.TotalMilliseconds - previous.TotalProcessorTime.TotalMilliseconds;
            Load = DiffTotalProcessorTime / deltaTime.TotalMilliseconds;
            IsDiffSet = true;
        }

        public override string ToString()
        {
            var sb = new StringBuilder();
            sb.Append($"Id:{Id,5}");
            sb.Append($" T:{TotalProcessorTime.ToString(fmt)}");
            if (IsDiffSet)
            {
                var loadStr = $"{Load * 100.0:0.0}";
                sb.Append($" L:{loadStr,5}%");
            }
            return sb.ToString();            
        }
    }
}
