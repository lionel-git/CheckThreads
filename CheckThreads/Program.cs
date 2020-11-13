using System;

namespace CheckThreads
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                ThreadInfo.DisplayState = true;
                if (args.Length > 0)
                {
                    foreach (var arg in args)
                    {
                        try
                        {
                            var threadChecker = new ThreadChecker(arg);
                            threadChecker.Check();
                        }
                        catch (Exception e)
                        {
                            Console.WriteLine($"{arg}: {e.Message}");
                        }
                    }
                }
                else
                    Console.WriteLine("Syntax: prg <pid> or <program name>");
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }
    }
}
