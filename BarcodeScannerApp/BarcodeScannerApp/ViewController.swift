//
//  ViewController.swift
//  BarcodeScannerApp
//
//  Created by Justin Frankle on 2/26/20.
//  Copyright © 2020 Justin Frankle. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    
    @IBOutlet weak var dataLabel: UILabel!
    @IBOutlet weak var descriptionLabel: UILabel!
    @IBOutlet weak var scanProptText: UILabel!
    @IBOutlet weak var scanButton: UIButton!
    @IBOutlet weak var codeBox: UITextField!
    @IBOutlet weak var imageBox: UIImageView!
    
    var currentCode = "748927021356"
    let currentKey = "tvt9qkfa5i9vgigiq46qzy79dx6mu2"
    
    var productImages = [] as [String]
    var currentImage = 0
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        dataLabel.numberOfLines = 0
        dataLabel.lineBreakMode = NSLineBreakMode.byWordWrapping
        descriptionLabel.numberOfLines = 0
        descriptionLabel.lineBreakMode = NSLineBreakMode.byWordWrapping
        //makeCall()
        // Do any additional setup after loading the view.
    }
    
    
    @IBAction func codeChanged(_ sender: Any) {
        currentCode = codeBox.text!
        print("Current code is: " + currentCode)
    }
    
    @IBAction func startScan(_ sender: Any) {
        makeCall()
    }
    
    
    @IBAction func cycleImageLeft(_ sender: Any) {
        print("left")
        
        if currentImage == 0 {
            currentImage = productImages.count - 1
        }
        else {
            currentImage -= 1
        }
        
        let imageURL = URL(string: productImages[currentImage])
        
        DispatchQueue.main.async {
            self.updateImage(url: imageURL!)
        }
    }
    
    @IBAction func cycleImageRight(_ sender: Any) {
        print("right")
        
        if currentImage == productImages.count - 1 {
            currentImage = 0
        }
        else {
            currentImage += 1
        }
        
        let imageURL = URL(string: productImages[currentImage])
        
        DispatchQueue.main.async {
            self.updateImage(url: imageURL!)
        }
    }
    
    func updateImage (url: URL) {
        DispatchQueue.global().async { [weak self] in
            if let data = try? Data(contentsOf: url) {
                if let image = UIImage(data: data) {
                    DispatchQueue.main.async {
                        self?.imageBox.image = image
                    }
                }
            }
        }
    }
    
    func updateLabel (text: String, desc: String, url: URL) {
        dataLabel.text = text
        descriptionLabel.text = desc
        //scanProptText.text = "Barcode Scanned"
        
        DispatchQueue.global().async { [weak self] in
            if let data = try? Data(contentsOf: url) {
                if let image = UIImage(data: data) {
                    DispatchQueue.main.async {
                        self?.imageBox.image = image
                    }
                }
            }
        }
    }
    
    func makeCall () {
        //let todoEndpoint: String = "https://cat-fact.herokuapp.com/facts/random?animal_type=cat&amount=1"
        let todoEndpoint: String = "https://api.barcodelookup.com/v2/products?barcode=" + currentCode +  "&formatted=y&key=" + currentKey
        guard let url = URL(string: todoEndpoint) else {
          print("Error: cannot create URL")
          return
        }
        let urlRequest = URLRequest(url: url)
        
        let session = URLSession.shared
        
        let task = session.dataTask(with: urlRequest) {
          (data, response, error) in
          // check for any errors
          guard error == nil else {
            print("error calling GET on /todos/1")
            print(error!)
            return
          }
          // make sure we got data
          guard let responseData = data else {
            print("Error: did not receive data")
            return
          }
          // parse the result as JSON, since that's what the API provides
          do {
            guard let todo = try JSONSerialization.jsonObject(with: responseData, options: .allowFragments)
              as? [String: Any] else {
              print("error trying to convert data to JSON")
              return
            }
            
            // now we have the todo
            // let's just print it to prove we can access it
            print("The todo is: " + todo.description)
            
            
            var title = ""
            var description = ""
            var imageURL = URL(string: "https://www.zerotoappstore.com/")
            if let retriever = todo["products"] as? [[String: AnyObject]] {
                for product in retriever {

                    if let field = product["product_name"] as? String {
                        title = field
                    }
                    
                    if let field = product["description"] as? String {
                        description = field
                    }
                    
                    //if let field = product[]
                    if let images = product["images"] as? [String] {
                        self.productImages = images
                        imageURL = URL(string: images[0])
                    }
                }
            }
            
            //self.dataLabel.text = todo.description
            DispatchQueue.main.async {
                // Do all your UI stuff here
                self.updateLabel(text: title, desc: description, url: imageURL!)
            }
            
            // the todo object is a dictionary
            // so we just access the title using the "title" key
            // so check for a title and print it if we have one
            guard let todoTitle = title as? String else {
              print("Could not get todo title from JSON")
              return
            }
            print("The title is: " + todoTitle)
          } catch  {
            print("error trying to convert data to JSON")
            return
          }
        }
        task.resume()
        
    }
    

    
    
    

}

