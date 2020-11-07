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
        const string fmt = @"hh\:mm\:ss";

        public static bool DisplayState { get; set; }

        public int Id { get; set; }
        public TimeSpan TotalProcessorTime { get; set; }
        public IntPtr StartAddress { get; set; }

        public ThreadState ThreadState { get;set;}
        public string WaitReason { get; set; }
        
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
            ThreadState = processThread.ThreadState;
            if (processThread.ThreadState == ThreadState.Wait)
                WaitReason = processThread.WaitReason.ToString();
            StartAddress = processThread.StartAddress;
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
                sb.Append($" L:{Helpers.FD(Load*100.0)}%");
            if (DisplayState)
            {
                sb.Append($" S:{ThreadState}");
                if (!string.IsNullOrEmpty(WaitReason))
                    sb.Append($" W:{WaitReason}");
            }
            return sb.ToString();            
        }
    }
}
