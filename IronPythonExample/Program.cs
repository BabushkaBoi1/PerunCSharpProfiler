using IronPython.Hosting;
using Microsoft.Scripting.Hosting;
using System;

class Program
{
    static void Main(string[] args)
    {
        var engine = Python.CreateEngine();

        // execute a Python script
        engine.Execute("print('Hello, world!')");

        // evaluate a Python expression
        var scope = engine.CreateScope();
        scope.SetVariable("result", engine.Execute("2 + 2"));

        // retrieve the value of the expression from the scope
        var value = scope.GetVariable("result").ToString();
        Console.WriteLine("The result is: " + value);
    }
}