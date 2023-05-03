﻿using CacheManager.Core;
using CacheManager.Core.Logging;
using CacheManager.Core.Utility;
using Unity;
using Unity.Injection;
using Unity.Lifetime;

namespace CacheManager.Examples
{
    public class Program
    {
        private static void Main()
        {
            EventsExample();
            UnityInjectionExample();
            UnityInjectionExample_Advanced();
            SimpleCustomBuildConfigurationUsingConfigBuilder();
            SimpleCustomBuildConfigurationUsingFactory();
            UpdateTest();
            UpdateCounterTest();
        }

        private static void EventsExample()
        {
            var cache = CacheFactory.Build<string>(s => s.WithDictionaryHandle());
            cache.OnAdd += (sender, args) => Console.WriteLine("Added " + args.Key);
            cache.OnGet += (sender, args) => Console.WriteLine("Got " + args.Key);
            cache.OnRemove += (sender, args) => Console.WriteLine("Removed " + args.Key);

            cache.Add("key", "value");
            var val = cache.Get("key");
            cache.Remove("key");
        }

        private static void SimpleCustomBuildConfigurationUsingConfigBuilder()
        {
            // this is using the CacheManager.Core.Configuration.ConfigurationBuilder to build a
            // custom config you can do the same with the CacheFactory
            var cfg = ConfigurationBuilder.BuildConfiguration(settings =>
            {
                settings.WithUpdateMode(CacheUpdateMode.Up)
                    .WithDictionaryHandle()
                        .EnablePerformanceCounters()
                        .WithExpiration(ExpirationMode.Sliding, TimeSpan.FromSeconds(10));
            });

            var cache = CacheFactory.FromConfiguration<string>(cfg);
            cache.Add("key", "value");

            // reusing the configuration and using the same cache for different types:
            var numbers = CacheFactory.FromConfiguration<int>(cfg);
            numbers.Add("intKey", 2323);
            numbers.Update("intKey", v => v + 1);
        }

        private static void SimpleCustomBuildConfigurationUsingFactory()
        {
            var cache = CacheFactory.Build(settings =>
            {
                settings
                    .WithUpdateMode(CacheUpdateMode.Up)
                    .WithDictionaryHandle()
                        .EnablePerformanceCounters()
                        .WithExpiration(ExpirationMode.Sliding, TimeSpan.FromSeconds(10));
            });

            cache.Add("key", "value");
        }

        private static void UnityInjectionExample()
        {
            var container = new UnityContainer();
            container.RegisterType<ICacheManager<object>>(
                new ContainerControlledLifetimeManager(),
                new InjectionFactory((c) => CacheFactory.Build(s => s.WithDictionaryHandle())));

            container.RegisterType<UnityInjectionExampleTarget>();

            // resolving the test target object should also resolve the cache instance
            var target = container.Resolve<UnityInjectionExampleTarget>();
            target.PutSomethingIntoTheCache();

            // our cache manager instance should still be there so should the object we added in the
            // previous step.
            var checkTarget = container.Resolve<UnityInjectionExampleTarget>();
            checkTarget.GetSomething();
        }

        private static void UnityInjectionExample_Advanced()
        {
            var container = new UnityContainer();
            container.RegisterType(
                typeof(ICacheManager<>),
                new ContainerControlledLifetimeManager(),
                new InjectionFactory(
                    (c, t, n) => CacheFactory.FromConfiguration(
                        t.GetGenericArguments()[0],
                        ConfigurationBuilder.BuildConfiguration(cfg => cfg.WithDictionaryHandle()))));

            var stringCache = container.Resolve<ICacheManager<string>>();

            // testing if we create a singleton instance per type, every Resolve of the same type should return the same instance!
            var stringCacheB = container.Resolve<ICacheManager<string>>();
            stringCache.Put("key", "something");

            var intCache = container.Resolve<ICacheManager<int>>();
            var intCacheB = container.Resolve<ICacheManager<int>>();
            intCache.Put("key", 22);

            var boolCache = container.Resolve<ICacheManager<bool>>();
            var boolCacheB = container.Resolve<ICacheManager<bool>>();
            boolCache.Put("key", false);

            Console.WriteLine("Value type is: " + stringCache.GetType().GetGenericArguments()[0].Name + " test value: " + stringCacheB["key"]);
            Console.WriteLine("Value type is: " + intCache.GetType().GetGenericArguments()[0].Name + " test value: " + intCacheB["key"]);
            Console.WriteLine("Value type is: " + boolCache.GetType().GetGenericArguments()[0].Name + " test value: " + boolCacheB["key"]);
        }

        private static void UpdateTest()
        {
            var cache = CacheFactory.Build<string>(s => s.WithDictionaryHandle());

            Console.WriteLine("Testing update...");

            if (!cache.TryUpdate("test", v => "item has not yet been added", out string newValue))
            {
                Console.WriteLine("Value not added?: {0}", newValue == null);
            }

            cache.Add("test", "start");
            Console.WriteLine("Initial value: {0}", cache["test"]);

            cache.AddOrUpdate("test", "adding again?", v => "updating and not adding");
            Console.WriteLine("After AddOrUpdate: {0}", cache["test"]);

            cache.Remove("test");
            try
            {
                var removeValue = cache.Update("test", v => "updated?");
            }
            catch
            {
                Console.WriteLine("Error as expected because item didn't exist.");
            }

            // use try update to not deal with exceptions
            if (!cache.TryUpdate("test", v => v, out string removedValue))
            {
                Console.WriteLine("Value after remove is null?: {0}", removedValue == null);
            }
        }

        private static void UpdateCounterTest()
        {
            var cache = CacheFactory.Build<long>(s => s.WithDictionaryHandle());

            Console.WriteLine("Testing update counter...");

            cache.AddOrUpdate("counter", 0, v => v + 1);

            Console.WriteLine("Initial value: {0}", cache.Get("counter"));

            for (var i = 0; i < 12345; i++)
            {
                cache.Update("counter", v => v + 1);
            }

            Console.WriteLine("Final value: {0}", cache.Get("counter"));
        }
    }

    public class UnityInjectionExampleTarget
    {
        private ICacheManager<object> _cache;

        public UnityInjectionExampleTarget(ICacheManager<object> cache)
        {
            _cache = cache ?? throw new ArgumentNullException(nameof(cache));
        }

        public void GetSomething()
        {
            var value = _cache.Get("myKey");
            var x = value;
            if (value == null)
            {
                throw new InvalidOperationException();
            }
        }

        public void PutSomethingIntoTheCache()
        {
            _cache.Put("myKey", "something");
        }
    }
}
