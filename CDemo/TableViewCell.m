//
//  TableViewCell.m
//  Demo
//
//  Created by 赵江明 on 2019/11/8.
//  Copyright © 2019 Jiangmingz. All rights reserved.
//

#import "TableViewCell.h"

@implementation TableViewCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if(self) {
        self.textLabel.numberOfLines = 0;
        self.accessoryType = UITableViewCellAccessoryNone;
    }
    
    return self;
}

+ (NSString *)identifier {
    return NSStringFromClass([self class]);
}

- (CGSize)sizeThatFits:(CGSize)size {
    CGSize textSize = [self.textLabel sizeThatFits:self.bounds.size];
    return CGSizeMake(self.bounds.size.width, MAX(textSize.height, 44.0f));
}

@end
