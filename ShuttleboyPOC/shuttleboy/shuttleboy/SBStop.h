//
//  SBStop.h
//  shuttleboy
//
//  Created by LISA ROSSELLI on 11/2/13.
//  Copyright (c) 2013 LISA ROSSELLI. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SBStop : NSObject

@property (nonatomic, assign) int idNum;
@property (nonatomic, retain) NSString *name;
@property (nonatomic, assign) float latitude;
@property (nonatomic, assign) float longitude;

-(id)initWithId:(int)index;

@end
