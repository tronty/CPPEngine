﻿// -----------------------------------------------------------------------
//  <copyright file="PerformArithmeticExtension.cs">
//      Copyright (c) 2021 Dean Edis. All rights reserved.
//  </copyright>
//  <summary>
//  This code is provided on an "as is" basis and without warranty of any kind.
//  We do not warrant or make any representations regarding the use or
//  results of use of this code.
//  </summary>
// -----------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Linq;
using Shrinker.Lexer;
using Shrinker.Parser.SyntaxNodes;

namespace Shrinker.Parser.Optimizations
{
    public static class PerformArithmeticExtension
    {
        private const int MaxDp = 5;

        /// <summary>
        /// Returns true if all optimizations should be re-run.
        /// </summary>
        public static bool PerformArithmetic(this SyntaxNode rootNode)
        {
            var repeatSimplifications = false;

            while (true)
            {
                var didChange = false;

                // 'a = b + -c' => 'a = b - c'
                foreach (var symbolNode in rootNode.TheTree
                    .OfType<GenericSyntaxNode>()
                    .Where(
                           o => (o.Token as SymbolOperatorToken)?.Content == "+" &&
                                (o.Next?.Token as SymbolOperatorToken)?.Content == "-")
                    .ToList())
                {
                    symbolNode.Remove();
                    didChange = true;
                }

                // 'f + -2.3' => 'f - 2.3'
                foreach (var numNode in rootNode.TheTree
                    .OfType<GenericSyntaxNode>()
                    .Where(
                           o => o.Token is INumberToken &&
                                o.Token.Content.StartsWith("-") &&
                                o.Previous?.Token?.GetMathSymbolType() == TokenExtensions.MathSymbolType.AddSubtract)
                    .ToList())
                {
                    var symbol = numNode.Previous.Token.Content[0] == '-' ? "+" : "-";
                    numNode.Previous.ReplaceWith(new GenericSyntaxNode(new SymbolOperatorToken(symbol)));
                    ((INumberToken)numNode.Token).MakePositive();
                    didChange = true;
                }

                // 'f * 1.0' or 'f / 1.0' => 'f'
                foreach (var oneNode in rootNode.TheTree
                    .OfType<GenericSyntaxNode>()
                    .Where(
                           o => (o.Token as INumberToken)?.IsOne() == true &&
                                o.Previous?.Token?.GetMathSymbolType() == TokenExtensions.MathSymbolType.MultiplyDivide)
                    .ToList())
                {
                    oneNode.Previous.Remove();
                    oneNode.Remove();
                    didChange = true;
                }
                
                // 'f / 0.0' => 'f * 0.0'
                foreach (var zeroNode in rootNode.TheTree
                    .OfType<GenericSyntaxNode>()
                    .Where(
                           o => (o.Token as INumberToken)?.IsZero() == true &&
                                o.Previous?.Token?.Content == "/")
                    .ToList())
                {
                    zeroNode.Previous.ReplaceWith(new GenericSyntaxNode(new SymbolOperatorToken("*")));
                    didChange = true;
                }
                
                // 'f * 0.0' => <Nothing>
                if (!didChange)
                {
                    foreach (var zeroNode in rootNode.TheTree
                        .OfType<GenericSyntaxNode>()
                        .Where(
                               o => (o.Token as INumberToken)?.IsZero() == true &&
                                    o.Previous?.Token?.Content == "*" &&
                                    (o.Next == null || o.Next?.Token is CommaToken))
                        .ToList())
                    {
                        var f = zeroNode.Previous.Previous;
                        if ((f as GenericSyntaxNode)?.Token is AlphaNumToken ||
                            f is FunctionCallSyntaxNode call && !call.HasOutParam)
                        {
                            f.Remove();
                            zeroNode.Previous.Remove();
                            didChange = true;
                        }
                    }
                }
                
                // 'f + 0.0' => f
                if (!didChange)
                {
                    foreach (var zeroNode in rootNode.TheTree
                        .OfType<GenericSyntaxNode>()
                        .Where(
                               o => (o.Token as INumberToken)?.IsZero() == true &&
                                    o.Previous?.Token?.GetMathSymbolType() == TokenExtensions.MathSymbolType.AddSubtract &&
                                    (o.Next == null || o.Next?.Token is CommaToken))
                        .ToList())
                    {
                        zeroNode.Previous.Remove();
                        zeroNode.Remove();
                        didChange = true;
                    }
                }

                // pow(1.1, 2.2) => <the result>
                var functionCalls = rootNode.TheTree.OfType<GlslFunctionCallSyntaxNode>().ToList();
                foreach (var powNode in functionCalls
                    .Where(o => o.Name == "pow" && o.Params.IsNumericCsv())
                    .ToList())
                {
                    var xy = powNode.Params.Children.Where(o => o.Token is FloatToken).Select(o => ((FloatToken)o.Token).Number).ToList();
                    if (xy.Count == 2 && xy.All(o => o > 0.0))
                    {
                        powNode.Params.Remove();
                        powNode.ReplaceWith(new GenericSyntaxNode(FloatToken.From(Math.Pow(xy[0], xy[1]), MaxDp)));
                        didChange = true;
                    }
                }

                // clamp(n, min, max) => <the result>
                foreach (var clampNode in functionCalls
                    .Where(o => o.Name == "clamp" && o.Params.IsNumericCsv())
                    .ToList())
                {
                    var xyz = clampNode.Params.Children.Where(o => o.Token is FloatToken).Select(o => ((FloatToken)o.Token).Number).ToList();
                    if (xyz.Count == 3)
                    {
                        clampNode.Params.Remove();
                        clampNode.ReplaceWith(new GenericSyntaxNode(FloatToken.From(Math.Min(Math.Max(xyz[0], xyz[1]), xyz[2]), MaxDp)));
                        didChange = true;
                    }
                }

                // length(vecN) => <the result>
                foreach (var lengthNode in functionCalls
                    .Where(o => o.Name == "length" && o.Params.Children.Count == 2)
                    .ToList())
                {
                    var nums = GetVectorNumericCsv(lengthNode.Params.Children.First());
                    if (nums == null)
                        continue;

                    var len = Math.Sqrt(nums.Sum(o => o * o));
                    lengthNode.ReplaceWith(new GenericSyntaxNode(FloatToken.From(len, MaxDp)));
                    didChange = true;
                }

                // normalize(vecN) => <the result>
                foreach (var normalizeNode in functionCalls
                    .Where(o => o.Name == "normalize" && o.Params.Children.Count == 2)
                    .ToList())
                {
                    var nums = GetVectorNumericCsv(normalizeNode.Params.Children.First());
                    if (nums == null)
                        continue;

                    var mag = Math.Sqrt(nums.Sum(o => o * o));

                    // Squash vecN(n, n, n, ...) down to vecN(n)
                    if (nums.Count > 1 && nums.Distinct().Count() == 1)
                        nums = new List<double> { nums.First() };

                    var newVectorNumNodes = nums.SelectMany(o => new[]
                    {
                        new GenericSyntaxNode(new CommaToken()),
                        new GenericSyntaxNode(FloatToken.From(o / mag, MaxDp).AsIntIfPossible())
                    }).Skip(1);

                    var newVectorBrackets = new RoundBracketSyntaxNode(newVectorNumNodes);
                    var newVectorNode = new GenericSyntaxNode(normalizeNode.Params.Children.First().Token.Content);
                    normalizeNode.ReplaceWith(newVectorNode).InsertNextSibling(newVectorBrackets);
                    didChange = true;
                }

                // Constant math/trig functions => <the result>
                var mathOp = new List<Tuple<string, Func<double, double>>>
                {
                    new Tuple<string, Func<double, double>>("abs", Math.Abs),
                    new Tuple<string, Func<double, double>>("length", Math.Abs),
                    new Tuple<string, Func<double, double>>("sqrt", d => Math.Sqrt(Math.Abs(d))),
                    new Tuple<string, Func<double, double>>("sin", Math.Sin),
                    new Tuple<string, Func<double, double>>("cos", Math.Cos),
                    new Tuple<string, Func<double, double>>("tan", Math.Tan),
                    new Tuple<string, Func<double, double>>("asin", Math.Asin),
                    new Tuple<string, Func<double, double>>("acos", Math.Acos),
                    new Tuple<string, Func<double, double>>("atan", Math.Atan),
                    new Tuple<string, Func<double, double>>("radians", x => x / 180.0 * Math.PI),
                    new Tuple<string, Func<double, double>>("degrees", x => x / Math.PI * 180.0),
                    new Tuple<string, Func<double, double>>("sign", x => Math.Sign(x)),
                    new Tuple<string, Func<double, double>>("floor", Math.Floor),
                    new Tuple<string, Func<double, double>>("ceil", Math.Ceiling),
                    new Tuple<string, Func<double, double>>("trunc", Math.Truncate),
                    new Tuple<string, Func<double, double>>("fract", x => x - Math.Truncate(x))
                };

                foreach (var mathNode in functionCalls
                    .Where(o => mathOp.Select(op => op.Item1).Contains(o.Name) && o.Params.IsNumericCsv())
                    .ToList())
                {
                    var x = mathNode.Params.Children.Where(o => o.Token is FloatToken).Select(o => ((FloatToken)o.Token).Number).ToList();
                    if (x.Count == 1)
                    {
                        mathNode.Params.Remove();
                        var mathFunc = mathOp.Single(o => o.Item1 == mathNode.Name).Item2;
                        mathNode.ReplaceWith(new GenericSyntaxNode(FloatToken.From(mathFunc(x[0]), MaxDp)));
                        didChange = true;
                    }
                }

                // 'vecN(...) + ...' or '... + vecN(...)'
                foreach (var vectorNode in rootNode.TheTree
                    .OfType<GenericSyntaxNode>()
                    .Where(o => o.Token is TypeToken t && t.IsVector()))
                {
                    // Find the brackets.
                    if (vectorNode.Next is not RoundBracketSyntaxNode brackets)
                        continue;

                    // Brackets must be filled with floats.
                    if (!brackets.IsNumericCsv())
                        continue;

                    // vector then float.
                    {
                        // Find the math symbol.
                        var symbolNode = brackets.Next;
                        if (symbolNode?.Token is SymbolOperatorToken symbol && symbol.GetMathSymbolType() != TokenExtensions.MathSymbolType.Unknown)
                        {
                            // Find the float number.
                            var numberNode = symbolNode.Next;
                            if (numberNode?.Token is FloatToken)
                            {
                                if (IsSafeToPerformMath(vectorNode, symbol, numberNode))
                                {
                                    // Perform math on each bracketed value.
                                    var newCsv =
                                        brackets
                                            .GetCsv()
                                            .Select(o => DoNodeMath(o.Single(), symbolNode, numberNode))
                                            .Select(o => new GenericSyntaxNode(FloatToken.From(o, MaxDp).AsIntIfPossible()));

                                    // Replace bracket content and sum (if it shrinks the code).
                                    var newBrackets = new RoundBracketSyntaxNode(newCsv.ToCsv());
                                    var oldSize = brackets.ToCode().Length + 1 + numberNode.ToCode().Length;
                                    var newSize = newBrackets.ToCode().Length;
                                    if (newSize <= oldSize)
                                    {
                                        brackets.ReplaceWith(newBrackets);
                                        symbolNode.Remove();
                                        numberNode.Remove();

                                        didChange = true;
                                        continue;
                                    }
                                }
                            }
                        }
                    }

                    // float then vector.
                    {
                        // Find the math symbol.
                        var symbolNode = vectorNode.Previous;
                        if (symbolNode?.Token is SymbolOperatorToken symbol && symbol.GetMathSymbolType() != TokenExtensions.MathSymbolType.Unknown)
                        {
                            // Find the float number.
                            var numberNode = symbolNode.Previous;
                            if (numberNode?.Token is FloatToken)
                            {
                                if (IsSafeToPerformMath(numberNode, symbol, vectorNode))
                                {
                                    // Perform math on each bracketed value.
                                    var newCsv =
                                        brackets
                                            .GetCsv()
                                            .Select(o => DoNodeMath(numberNode, symbolNode, o.Single()))
                                            .Select(o => new GenericSyntaxNode(FloatToken.From(o, MaxDp).AsIntIfPossible()));

                                    // Replace bracket content and sum (if it shrinks the code).
                                    var newBrackets = new RoundBracketSyntaxNode(newCsv.ToCsv());
                                    var oldSize = brackets.ToCode().Length + 1 + numberNode.ToCode().Length;
                                    var newSize = newBrackets.ToCode().Length;
                                    if (newSize <= oldSize)
                                    {
                                        brackets.ReplaceWith(newBrackets);
                                        symbolNode.Remove();
                                        numberNode.Remove();

                                        didChange = true;
                                        continue;
                                    }
                                }
                            }
                        }
                    }

                    // vector then vector.
                    {
                        // Find the math symbol.
                        var symbolNode = brackets.Next;
                        if (symbolNode?.Token is SymbolOperatorToken symbol && symbol.GetMathSymbolType() != TokenExtensions.MathSymbolType.Unknown)
                        {
                            // Find the second vector.
                            var rhsVectorNode = symbolNode.Next;
                            if (rhsVectorNode?.Token is TypeToken t && t.IsVector())
                            {
                                var rhsVectorBrackets = rhsVectorNode.Next as RoundBracketSyntaxNode;
                                if (IsSafeToPerformMath(vectorNode, symbol, rhsVectorNode))
                                {
                                    // Ensure both brackets have the same number of elements.
                                    var lhsCsv = brackets.GetCsv().ToList();
                                    var rhsCsv = rhsVectorBrackets.GetCsv().ToList();
                                    while (lhsCsv.Count < rhsCsv.Count)
                                        lhsCsv.Add(new[] { lhsCsv.First().Single().Clone() });
                                    while (rhsCsv.Count < lhsCsv.Count)
                                        rhsCsv.Add(new[] { rhsCsv.First().Single().Clone() });

                                    // Perform math on each bracketed value.
                                    var newCsv =
                                        lhsCsv
                                            .Select((o, i) => DoNodeMath(o.Single(), symbolNode, rhsCsv[i].Single()))
                                            .Select(o => new GenericSyntaxNode(FloatToken.From(o, MaxDp).AsIntIfPossible()));

                                    // Replace bracket content and sum.
                                    brackets.ReplaceWith(new RoundBracketSyntaxNode(newCsv.ToCsv()));
                                    symbolNode.Remove();
                                    rhsVectorNode.Remove();
                                    rhsVectorBrackets.Remove();

                                    didChange = true;
                                }
                            }
                        }
                    }
                }

                // Perform simple arithmetic calculations.
                foreach (var numNodeA in rootNode.TheTree
                    .OfType<GenericSyntaxNode>()
                    .Where(
                           o => o.Token is INumberToken &&
                                o.Next?.Token?.GetMathSymbolType() != TokenExtensions.MathSymbolType.Unknown &&
                                o.Next?.Next?.Token is INumberToken)
                    .Reverse()
                    .ToList())
                {
                    var symbolNode = numNodeA.Next;
                    var numNodeB = symbolNode.Next;

                    if (!IsSafeToPerformMath(numNodeA, symbolNode.Token as SymbolOperatorToken, numNodeB))
                        continue;

                    var c = DoNodeMath(numNodeA, symbolNode, numNodeB);

                    var isFloatResult = numNodeA.Token is FloatToken || numNodeB.Token is FloatToken;
                    numNodeB.Remove();
                    symbolNode.Remove();

                    var newNode = numNodeA.ReplaceWith(new GenericSyntaxNode(isFloatResult ? FloatToken.From(c, MaxDp) : new IntToken((int)c)));

                    // If new node is the sole child of a group, promote it.
                    if (newNode.IsOnlyChild() && newNode.Parent.GetType() == typeof(GroupSyntaxNode))
                        newNode.Parent.ReplaceWith(newNode);

                    didChange = true;
                }

                if (!didChange)
                    break;

                repeatSimplifications = true;
            }

            return repeatSimplifications;
        }

        private static List<double> GetVectorNumericCsv(SyntaxNode vectorNode)
        {
            if (vectorNode.Next is not RoundBracketSyntaxNode brackets || !brackets.IsNumericCsv())
                return null;

            // Get vector type.
            int c;
            switch (vectorNode.Token?.Content)
            {
                case "vec2":
                    c = 2;
                    break;
                case "vec3":
                    c = 3;
                    break;
                case "vec4":
                    c = 4;
                    break;
                default:
                    return null;
            }

            // Support vectors with single component (E.g. vec3(x)).
            var nums = brackets.GetCsv().Select(o => double.Parse(o.Single().Token.Content)).ToList();
            while (nums.Count < c)
                nums.Add(nums.Last());
            return nums;
        }

        private static bool IsSafeToPerformMath(SyntaxNode lhsNumNode, SymbolOperatorToken symbol, SyntaxNode rhsNumNode)
        {
            if (lhsNumNode == null || symbol == null || rhsNumNode == null)
                return false;

            var symbolType = symbol.GetMathSymbolType();
            switch (symbolType)
            {
                case TokenExtensions.MathSymbolType.Unknown:
                    return false;

                // If main symbol is * or /, that's fine - Any further symbols in the expression won't affect things.
                case TokenExtensions.MathSymbolType.MultiplyDivide:
                    return true;

                // Only ok to perform math on the two nodes if the surrounding symbols are + or - (or don't exist).
                case TokenExtensions.MathSymbolType.AddSubtract:
                    var prevSymbol = (lhsNumNode.Previous as GenericSyntaxNode)?.Token as SymbolOperatorToken;
                    var nextSymbol = (rhsNumNode.Next is RoundBracketSyntaxNode ? rhsNumNode.Next.Next : rhsNumNode.Next)?.Token as SymbolOperatorToken;

                    if (prevSymbol?.GetMathSymbolType() == TokenExtensions.MathSymbolType.Unknown)
                        prevSymbol = null;
                    if (nextSymbol?.GetMathSymbolType() == TokenExtensions.MathSymbolType.Unknown)
                        nextSymbol = null;

                    return new[] { prevSymbol, nextSymbol }.Where(o => o != null).All(o => o.GetMathSymbolType() == symbolType);

                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        private static double DoNodeMath(SyntaxNode numNodeA, SyntaxNode symbolNode, SyntaxNode numNodeB)
        {
            var a = double.Parse(numNodeA.Token.Content);
            var b = double.Parse(numNodeB.Token.Content);

            var symbol = symbolNode.Token.Content[0];

            // Invert * or / if preceded by a /.
            // E.g. 1.2 / 2.3 * 4.5 = 1.2 / (2.3 / 4.5)
            //                      = 1.2 / 0.51111
            //                      = 2.3478
            if (numNodeA.Previous?.Token?.GetMathSymbolType() == TokenExtensions.MathSymbolType.MultiplyDivide &&
                numNodeA.Previous.HasNodeContent("/"))
            {
                symbol = symbol == '*' ? '/' : '*';
            }

            // Invert + or - if preceded by a -.
            // E.g. -3.0 + 0.1 = - (3.0 - 0.1)
            //                 = - (2.9)
            //                 = -2.9
            if (numNodeA.Previous.HasNodeContent("-") &&
                symbolNode.Token.GetMathSymbolType() == TokenExtensions.MathSymbolType.AddSubtract)
            {
                symbol = symbol == '+' ? '-' : '+';
            }

            switch (symbol)
            {
                case '+':
                    return a + b;
                case '-':
                    return a - b;
                case '*':
                    return a * b;
                case '/':
                    var c = a / b;
                    if (double.IsInfinity(c))
                        c = 0.0;
                    return c;
                default:
                    throw new InvalidOperationException($"Unrecognized math operation '{symbol}'.");
            }
        }
    }
}