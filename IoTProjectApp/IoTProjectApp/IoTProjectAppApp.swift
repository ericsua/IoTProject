//
//  IoTProjectAppApp.swift
//  IoTProjectApp
//
//  Created by Eric Suardi on 20/01/22.
//

import SwiftUI
import UIKit
import BackgroundTasks


@main
struct IoTProjectAppApp: App {
    //@UIApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    
        
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}

//class AppDelegate: NSObject, UIApplicationDelegate {
//    @State var datas = [APIdata]()
//    @State var values = Array(repeating: 0, count: 128)
//    @ObservedObject var notificationManager = LocalNotificationManager()
//
//    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey : Any]? = nil) -> Bool {
//        //application.setMinimumBackgroundFetchInterval(5)
//        print("Your code here")
//        registerForPushNotifications()
//
//
//
//        let bgTaskIdentifier = "eric.suardi.IoTProjectApp.refresh"
//        BGTaskScheduler.shared.register(forTaskWithIdentifier: bgTaskIdentifier, using: nil) { task in
//            print("first bg")
//            self.handleAppRefreshTask(task: task as! BGAppRefreshTask)
//           //let request = BGAppRefreshTaskRequest(identifier: bgTaskIdentifier)
//           // Fetch no earlier than 15 minutes from now
//           //request.earliestBeginDate = Date(timeIntervalSinceNow: 10)
//
////            do {
////                  try BGTaskScheduler.shared.submit(request)
////               } catch {
////                  print("Could not schedule app refresh: \(error)")
////               }
////
////               // Create an operation that performs the main part of the background task
////               let operation = RefreshAppContentsOperation()
////
////               // Provide an expiration handler for the background task
////               // that cancels the operation
////               task.expirationHandler = {
////                  operation.cancel()
////               }
////
////               // Inform the system that the background task is complete
////               // when the operation completes
////               operation.completionBlock = {
////                  task.setTaskCompleted(success: !operation.isCancelled)
////               }
////
////               // Start the operation
////               operationQueue.addOperation(operation)
//            //self.scheduleImagefetcher()
//
//        }
//
//        return true
//    }
//
//    func application(_ application: UIApplication, performFetchWithCompletionHandler completionHandler: @escaping (UIBackgroundFetchResult) -> Void) {
//        // fetch data from internet now
//        print("Fetchhing")
//        Api().loadData { (datas) in
//            print("got data in background")
//            self.datas.insert(datas, at: 0)
//            var i = 0
//            for d in datas.data.split(separator: ",") {
//                self.values[i] = Int(d) ?? 0
//                i+=1
//                print(d)
//            }
//
//        }
//
//    }
//
//    func applicationDidEnterBackground(_ application: UIApplication) {
//        print("DidEnterBackG")
//        //scheduleImagefetcher()
//        scheduleBackgroundPokemonFetch()
//    }
//
//    func scheduleImagefetcher() {
//        let request = BGProcessingTaskRequest(identifier: "eric.suardi.IoTProjectApp.refresh")
//        request.requiresNetworkConnectivity = true // Need to true if your task need to network process. Defaults to false.
//        request.requiresExternalPower = false
//        //If we keep requiredExternalPower = true then it required device is connected to external power.
//
//        request.earliestBeginDate = Date(timeIntervalSinceNow: 1 * 30) // fetch Image Count after 1 minute.
//        //Note :: EarliestBeginDate should not be set to too far into the future.
//        do {
//            try BGTaskScheduler.shared.submit(request)
//        } catch {
//            print("Could not schedule image fetch: (error)")
//        }
//    }
//
//    func scheduleBackgroundPokemonFetch() {
//        let pokemonFetchTask = BGAppRefreshTaskRequest(identifier: "eric.suardi.IoTProjectApp.refresh")
//        pokemonFetchTask.earliestBeginDate = Date(timeIntervalSinceNow: 5)
//        do {
//          try BGTaskScheduler.shared.submit(pokemonFetchTask)
//        } catch {
//          print("Unable to submit task: \(error.localizedDescription)")
//        }
//    }
//
//    func handleAppRefreshTask(task: BGAppRefreshTask) {
//        print("handleAppRefresh")
//        task.expirationHandler = {
//            task.setTaskCompleted(success: false)
//          }
//
//        Api().loadData { (datas) in
//            print("got data in background")
//            self.datas.insert(datas, at: 0)
//            var i = 0
//            for d in datas.data.split(separator: ",") {
//                self.values[i] = Int(d) ?? 0
//                i+=1
//            }
//
//        }
//
//
//        var maxNum = 0
//        for x in values {
//            print(x)
//            if x > 60 {
//                maxNum+=1
//
//            }
//        }
//        if maxNum > 2 {
//            //completionHandler(maxNum)
//            print("Max found! Sending notification")
//            self.notificationManager.sendNotification(title: "Hurray!", subtitle: nil, body: "If you see this text, launching the local notification worked!", launchIn: 1)
//
//        }
//
//        task.setTaskCompleted(success: true)
//
//        scheduleBackgroundPokemonFetch()
//    }
//
//    func sceneDidEnterBackground(_ scene: UIScene) {
//        print("sceneDidBBackground")
//        (UIApplication.shared.delegate as! AppDelegate).scheduleBackgroundPokemonFetch()
//    }
//
//
//    func registerForPushNotifications() {
//            UNUserNotificationCenter.current().requestAuthorization(options: [.alert, .sound, .badge]) { [weak self] granted, _ in
//                print("Permission granted: \(granted)")
//                guard granted else { return }
//                self?.getNotificationSettings()
//            }
//    }
//    func getNotificationSettings() {
//            UNUserNotificationCenter.current().getNotificationSettings { settings in
//                //print("Notification settings: \(settings)")
//                guard settings.authorizationStatus == .authorized else { return }
//                DispatchQueue.main.async {
//                    UIApplication.shared.registerForRemoteNotifications()
//                }
//            }
//        }
//}
//
//class LocalNotificationManager: ObservableObject {
//    var notifications = [Notification]()
//
//    func sendNotification(title: String, subtitle: String?, body: String, launchIn: Double) {
//
//        let content = UNMutableNotificationContent()
//        content.title = title
//        if let subtitle = subtitle {
//            content.subtitle = subtitle
//        }
//        content.body = body
//
//        let imageName = "logo"
//        guard let imageURL = Bundle.main.url(forResource: imageName, withExtension: "png") else { return }
//        let attachment = try! UNNotificationAttachment(identifier: imageName, url: imageURL, options: .none)
//        content.attachments = [attachment]
//
//        let trigger = UNTimeIntervalNotificationTrigger(timeInterval: launchIn, repeats: false)
//        let request = UNNotificationRequest(identifier: "demoNotification", content: content, trigger: trigger)
//        UNUserNotificationCenter.current().add(request, withCompletionHandler: nil)
//    }
//
//}
